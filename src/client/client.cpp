#include "client.h"

#include <packet.h>

#include <iostream>

#include "../constants.h"
#include "../utils.h"

void Client::getServerInfo() {
  Address stunServer(stunServerAddress, stunServerPort);

  uint32_t serverID = 0;
  std::cin >> serverID;

  std::cout << serverID << ":" << serverID << std::endl;
  Packet outPacket;
  outPacket << 'c' << serverID;

  sock_.sendTo(stunServer, outPacket);

  Packet inPacket;
  int end = recvFromTarget(stunServer, inPacket);

  if (end < 0)
    throw std::runtime_error("Error receiving message from stun server");

  uint32_t targetAddress = 0;
  uint16_t targetPort = 0;
  inPacket >> targetAddress >> targetPort;

  targetInfo_ = Address(targetAddress, targetPort);

  std::cout << targetAddress << ":" << targetPort << std::endl;
}

bool Client::connectToServer() {
  int attempts = 500;

  bool pinged = false;

  while (isRunning_ && attempts-- > 0) {
    Packet outPacket;
    std::string outmsg = pinged ? "pong" : "ping";
    outPacket << static_cast<int16_t>(Protocols::Connection) << outmsg;
    sock_.sendTo(targetInfo_, outPacket);

    Packet inPacket;

    int end = recvFromServer(Protocols::Connection, inPacket);

    if (end <= 0) {
      std::cout << WSAGetLastError() << "\n";
      Sleep(10);
      continue;
    }

    std::string msg;
    inPacket >> msg;

    if (msg == "ping") {
      pinged = true;
    }
    if (msg == "ping pong") {
      return true;
    }
  }

  return false;
}

void Client::run() {
  isRunning_ = true;
  getServerInfo();

  std::cout << targetInfo_.getIP() << " " << targetInfo_.getPort() << "\n";

  sock_.setNonBlockingMode();
  bool tryConnect = connectToServer();

  if (!tryConnect) {
    return;
  }

  connected_ = true;
  std::cout << "Connected\n";

  std::vector<char> buffer(1024);
  buffer[0] = '\0';

  while (isRunning_) {
    Packet inPacket;
    int end = recvFromServer(Protocols::Message, inPacket);

    if (end <= 0 || end >= buffer.size()) continue;

    recvMessages_.emplace_back(inPacket);
    std::cout << "Received from client: " << buffer.data();

    Sleep(10);
  }
}

void Client::sendMessage(const Packet& msg) {
  sock_.sendTo(targetInfo_, msg);
}

std::vector<Packet> Client::getMessages() {
  std::vector<Packet> packets = recvMessages_;
  recvMessages_.clear();
  return packets;
}

int Client::recvFromServer(Protocols protocol, Packet& inPacket) {
  return recvFromTarget(protocol, targetInfo_, inPacket);
}

// int Client::sendMsg(std::vector<char> &msg) {
//   int packet_size = send(sock_, msg.data(), msg.size(), 0);
//   if (packet_size == SOCKET_ERROR) {
//     std::cout << "Can't send message to Server. Error # " <<
//     WSAGetLastError()
//               << std::endl;
//     return 1;
//   }
//   return 0;
// }
//
// int Client::recvMsg(std::vector<char> &buffer) {
//   int packet_size = recv(sock_, buffer.data(), buffer.size(), 0);
//
//   if (packet_size == SOCKET_ERROR) {
//     std::cout << "Can't receive message from Server. Error # "
//               << WSAGetLastError() << std::endl;
//     return 1;
//   }
//   return 0;
// }
