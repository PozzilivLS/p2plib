#ifndef NETWORK_H
#define NETWORK_H

#include <memory>
#include <vector>

#include "address.h"
#include "socket.h"
#include "utils.h"

class Network {
 public:
  Network();
  virtual ~Network();

  int initialize();

  virtual void run() = 0;

 protected:
  int recvFromTarget(Address &target, Packet &inPacket);
  int recvFromTarget(Protocols protocol, Address &target, Packet &inPacket);

  int erStat_ = 0;

  in_addr ipToNum_{};
  //unique_ptr<Address> address_ = nullptr;

  Socket sock_;

  bool isRunning_ = false;
};

#endif