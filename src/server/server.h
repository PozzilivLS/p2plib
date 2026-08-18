#ifndef SERVER_H
#define SERVER_H

#include "../network.h"

#include <thread>
#include <mutex>
#include <unordered_map>

class Server : public Network {
 public:
  ~Server();

  void run() override;

  void sendMessage(const Packet &msg);
  void sendMessage(const uint32_t &client, const Packet &msg);

  std::vector<Packet> getMessagesFromClient(const uint32_t &client);

  const std::vector<uint32_t> getClients() const;

 private:
  void registerServer();

  bool connectToClient(Address &clientInfo);
  void handleClient(uint32_t clientId);

  int recvFromStun(Packet &inPacket);
  int recvFromClient(Protocols protocol, Address &clientInfo, Packet &inPacket);

  std::mutex clientsMutex_;

  Address stunServer_;
  std::unordered_map<uint32_t, Address> clients_;
  std::unordered_map<uint32_t, std::vector<Packet>> clientMessages_;
  std::vector<std::thread> clientThreads_;

  uint32_t myAddress_ = 0;
  uint16_t myPort_ = 0;
  uint32_t serverID_ = 0;
};

#endif