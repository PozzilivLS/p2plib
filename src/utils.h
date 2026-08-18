#pragma once

#include <cstdint>

#include <string>
#include <address.h>
#include "constants.h"

enum class Protocols {
  Connection = 1,
  Message = 2
};

inline uint32_t fnv1a(const void* data, size_t numBytes,
               unsigned int hash = FNV1A_SEED) {
  const byte* ptr = (const byte*)data;
  while (numBytes--) hash = (*ptr++ ^ hash) * FNV1A_PRIME;
  return hash;
}

inline uint32_t generateClientID(const Address& address) {
  std::string fullAddress = std::string(inet_ntoa(address.getInfo().sin_addr)) +
                            ":" + std::to_string(address.getInfo().sin_port);

  return fnv1a((void*)fullAddress.c_str(), fullAddress.size());
}