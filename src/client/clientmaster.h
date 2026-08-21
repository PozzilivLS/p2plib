#ifndef CLIENTMASTER_H
#define CLIENTMASTER_H

#include <thread>

#include "client.h"
#include <mutex>

class ClientMaster {
 public:
  void startClient();

  bool isConnected() { return client_.isConnected(); }

  void sendMessage(Packet &msg);

  std::vector<Packet> getIncomingMessages();

  void setServerID(uint32_t serverID);

 private:
  void managing();

  Client client_;
  std::thread clientThread_;
  std::thread managerThread_;

  std::mutex clientMutex_;

  int connectionCheckTimer_ = 5000;
};

#endif  // !CLIENTMASTER_H
