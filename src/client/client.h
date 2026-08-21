#ifndef CLIENT_H
#define CLIENT_H

#include "../network.h"

class Client : public Network {
 public:
  void getServerInfo(uint32_t serverID);
  bool connectToServer();

  void run() override;

  void sendMessage(const Packet &msg);

  std::vector<Packet> getMessages();
  
  bool isConnected() { return connected_; }

 private:
  int recvFromServer(Protocols protocol, Packet &inPacket);

  Address targetInfo_;

  std::vector<Packet> recvMessages_;

  bool connected_ = false;

  bool hasServerInfo_ = false;
};

#endif