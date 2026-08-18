#include "servermaster.h"

void ServerMaster::startServer() {
  server_.initialize();
  serverThread_ = std::thread(&Server::run, &server_);
  managerThread_ = std::thread(&ServerMaster::managing, this);
}

void ServerMaster::managing() {
  while (serverThread_.joinable()) {
    {
      Packet outPck;
      outPck << static_cast<int16_t>(Protocols::Connection);
      sendMessage(outPck);
    }

    Sleep(connectionCheckTimer_);
  }
}

void ServerMaster::sendMessage(Packet& msg) {
  std::lock_guard<std::mutex> lock(serverMutex_);
  server_.sendMessage(msg);
}

void ServerMaster::sendMessage(const uint32_t& client, Packet& msg) {
  std::lock_guard<std::mutex> lock(serverMutex_);
  server_.sendMessage(client, msg);
}

std::vector<Packet> ServerMaster::getIncomingMessagesFrom(
    const uint32_t& client) {
  std::lock_guard<std::mutex> lock(serverMutex_);
  return server_.getMessagesFromClient(client);
}

const std::vector<uint32_t> ServerMaster::getAllClients() const {
  return server_.getClients();
}
