#include "server.h"

#include <Packet.h>
#include <WS2tcpip.h>

#include <exception>
#include <iostream>

#include "../constants.h"
#include "../utils.h"

Server::~Server() { isRunning_ = false; }

void Server::registerServer() {
  stunServer_ = Address(stunServerAddress, stunServerPort);

  Packet outPacket;
  outPacket << 's';

  sock_.sendTo(stunServer_, outPacket);

  Packet inPacket;
  int end = recvFromStun(inPacket);
  // std::cout << end;
  if (end <= 0)
    throw std::runtime_error("Error receiving message from stun server");

  inPacket >> myAddress_ >> myPort_ >> serverID_;

  std::cout << myAddress_ << " : " << myPort_ << "\nID: " << serverID_
            << std::endl;
}

bool Server::connectToClient(Address &clientInfo) {
  int attempts = 500;

  bool ponged = false;

  while (isRunning_ && attempts-- > 0) {
    std::lock_guard<std::mutex> lock(clientsMutex_);

    Packet outPacket;
    std::string outmsg = ponged ? "ping pong" : "ping";
    outPacket << static_cast<int16_t>(Protocols::Connection) << outmsg;
    sock_.sendTo(clientInfo, outPacket);

    Packet inPacket;

    int end = recvFromClient(Protocols::Connection, clientInfo, inPacket);

    if (end <= 0) {
      std::cout << WSAGetLastError() << "\n";
      Sleep(10);
      continue;
    }

    std::string msg;
    inPacket >> msg;

    if (msg == "pong" && !ponged) {
      ponged = true;
      attempts = 10;
    }
  }

  return ponged;
}

void Server::handleClient(uint32_t clientId) {
  auto &clientInfo = clients_[clientId];
  bool tryConnect = connectToClient(clientInfo);

  if (!tryConnect) {
    std::cout << "not Connected\n";
    return;
  }
  std::cout << "Connected\n";

  std::vector<char> buffer(1024);

  while (isRunning_) {
    {
      std::lock_guard<std::mutex> lock(clientsMutex_);

      Packet inPacket;
      int end = recvFromClient(Protocols::Message, clientInfo, inPacket);

      if (end <= 0 || end >= buffer.size()) continue;

      clientMessages_[clientId].emplace_back(inPacket);
    }
    std::cout << "Received from client: " << buffer.data();

    Sleep(10);
  }
}

void Server::run() {
  isRunning_ = true;

  registerServer();

  sock_.setNonBlockingMode();

  while (isRunning_) {
    int end = 0;

    Packet inPacket;
    {
      std::lock_guard<std::mutex> lock(clientsMutex_);

      end = recvFromStun(inPacket);
    }
    if (end == 0) continue;
    if (end < 0) {
      continue;
      throw std::runtime_error("Error receiving message from stun server");
    }

    uint32_t clientAddress = 0;
    uint16_t clientPort = 0;
    inPacket >> clientAddress >> clientPort;

    Address clientInfo(clientAddress, clientPort);

    bool alreadyConnected = false;
    {
      std::lock_guard<std::mutex> lock(clientsMutex_);
      for (const auto &existing : clients_) {
        if (existing.second == clientInfo) {
          alreadyConnected = true;
          break;
        }
      }
    }
    if (alreadyConnected) continue;  // TODO: переделать

    std::cout << clientAddress << " : " << clientPort << " is connected"
              << std::endl;

    /*if (client == INVALID_SOCKET) {
      if (isRunning_) {
        std::cout << "Accept failed: " << WSAGetLastError() << std::endl;
      }
      continue;
    }*/

    std::cout << "Client connected!" << std::endl;

    uint32_t clientHash = generateClientID(clientInfo);
    clients_[clientHash] = clientInfo;

    clientThreads_.emplace_back(&Server::handleClient, this, clientHash);

    Sleep(10);
  }

  for (auto &thread : clientThreads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void Server::sendMessage(const Packet &msg) {
  std::lock_guard<std::mutex> lock(clientsMutex_);
  for (const auto &[id, client] : clients_) {
    sock_.sendTo(client, msg);
  }
}

void Server::sendMessage(const uint32_t &client, const Packet &msg) {
  std::lock_guard<std::mutex> lock(clientsMutex_);
  if (clients_.find(client) != clients_.end())
    sock_.sendTo(clients_[client], msg);
}

std::vector<Packet> Server::getMessagesFromClient(const uint32_t &client) { // переделать
  std::vector<Packet> packets = clientMessages_[client];
  clientMessages_[client].clear();
  return packets;
}

const std::vector<uint32_t> Server::getClients() const {
  std::vector<uint32_t> res;

  for (const auto &client : clients_) {
    res.emplace_back(client.first);
  }

  return res;
}

int Server::recvFromStun(Packet &inPacket) {
  return recvFromTarget(stunServer_, inPacket);
}

int Server::recvFromClient(Protocols protocol, Address &clientInfo,
                           Packet &inPacket) {
  return recvFromTarget(protocol, clientInfo, inPacket);
}

// int Server::sendMsg(std::vector<int> &msg) {
//   for (auto &conn : clientConns_) {
//     int packet_size = send(conn, msg.data(), msg.size(), 0);
//
//     if (packet_size == SOCKET_ERROR) {
//       std::cout << "Can't send message to Client. Error # " <<
//       WSAGetLastError()
//                 << std::endl;
//       return 1;
//     }
//   }
//   return 0;
// }
//
// int Server::recvMsg(std::vector<char> &buffer) { /*
//    int packet_size = recv(conn, buffer.data(), buffer.size(), 0);
//    std::cout << buffer.data() << "\n";*/
//
//   return 0;
// }
