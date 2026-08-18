#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef unsigned char byte;

const unsigned int FNV1A_PRIME = 0x01000193;
const unsigned int FNV1A_SEED = 0x811C9DC5;

inline const char* stunServerAddress = "194.87.99.134";
// inline const char* stunServerAddress = "192.168.56.105"; // vm
// inline const char* stunServerAddress = "10.0.2.15"; // vm
// inline const char* stunServerAddress = "127.0.0.1";
inline const unsigned short stunServerPort = 7000;

#endif