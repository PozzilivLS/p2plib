#include "network.h"

#include <exception>
#include <iostream>

Network::Network() { initializeSockets(); }

Network::~Network() { shutdownSockets(); }

int Network::initialize() {
  if (!sock_.create()) {
    std::cout << "Error initialization socket # " << WSAGetLastError()
              << std::endl;
    return 1;
  }

  std::cout << "Socket initialization is OK" << std::endl;

  unsigned short local_port;
  struct sockaddr_in sin;
  int addrlen = sizeof(sin);
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = 0;
  getsockname(sock_.getSocket(), (struct sockaddr*)&sin, &addrlen);

  local_port = ntohs(sin.sin_port);
  std::cout << local_port << std::endl;

  return 0;
}

int Network::recvFromTarget(Address& target, Packet& inPacket) {
  Address fromdata;
  int end = sock_.recvFrom(fromdata, inPacket);
  /*std::cout << "recvFromTarget: " << fromdata.getIP() << ":"
            << fromdata.getPort() << " target: " << target.getIP() << ":"
            << target.getPort() << std::endl;*/

  if (fromdata == target) {
    return end;
  }
  return 0;
}

int Network::recvFromTarget(Protocols protocol, Address& target,
                            Packet& inPacket) {
  Address fromdata;
  int end = sock_.recvFrom(fromdata, inPacket);

  if (end <= 0) {
    return end;
  }

  if (!(fromdata == target)) {
    return 0;
  }
  if (inPacket.getDataSize() < sizeof(int16_t)) {
    return 0;
  }

  int16_t prt = 0;
  inPacket >> prt;

  if (prt != static_cast<int16_t>(protocol)) {
    return 0;
  }

  return end;
}
