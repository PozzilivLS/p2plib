#include "clientmaster.h"

void ClientMaster::startClient() {
  client_.initialize();
  clientThread_ = std::thread(&Client::run, &client_);
  managerThread_ = std::thread(&ClientMaster::managing, this);
}

void ClientMaster::managing() {
  while (clientThread_.joinable()) {
    {
      Packet outPck;
      outPck << static_cast<int16_t>(Protocols::Connection);
      sendMessage(outPck);
    }

    Sleep(connectionCheckTimer_);
  }
}

void ClientMaster::sendMessage(Packet& msg) {
  std::lock_guard<std::mutex> lock(clientMutex_);
  client_.sendMessage(msg);
}


std::vector<Packet> ClientMaster::getIncomingMessages() {
  std::lock_guard<std::mutex> lock(clientMutex_);
  return client_.getMessages();
}

void ClientMaster::setServerID(uint32_t serverID) {
  client_.getServerInfo(serverID);
}
