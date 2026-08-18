#ifndef SERVERMASTER_H
#define SERVERMASTER_H

#include <thread>
#include "server.h"

class ServerMaster {
 public:
  void startServer();

  void sendMessage(Packet &msg);
  void sendMessage(const uint32_t &client, Packet &msg);
  std::vector<Packet> getIncomingMessagesFrom(const uint32_t &client);

  const std::vector<uint32_t> getAllClients() const;

 private:
  void managing();

  Server server_;
  std::thread serverThread_;
  std::thread managerThread_;

  std::mutex serverMutex_;

  int connectionCheckTimer_ = 5000;
};

#endif  // !SERVERMASTER_H
