#include "server/receiver.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sys/socket.h>


int segreceiver::rec() {
  buffer.resize(1024);
  int sig = recv(_socket, buffer.data(), 1024, 0);
  if (sig >= 1) {
    load.clear();
    msg = buffer[0];
    len = (static_cast<uint16_t>(buffer[2]) << 8) +
          static_cast<uint16_t>(buffer[3]);
    if (len > 0) {
      if (msg == 0x04) {
        tempsocket = static_cast<uint32_t>(buffer[4]) << 24 |
                     static_cast<uint32_t>(buffer[5]) << 16 |
                     static_cast<uint32_t>(buffer[6]) << 8 |
                     static_cast<uint32_t>(buffer[7]);
        int len = static_cast<uint32_t>(buffer[8]) << 8 |
                  static_cast<uint32_t>(buffer[9]);
        load.insert(load.end(), buffer.begin() + 10, buffer.end());

      } else {
        load.insert(load.end(), buffer.begin() + 4, buffer.end());
      }
    }
  }return sig;
}