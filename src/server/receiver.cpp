#include "server/receiver.hpp"
#include <cstddef>
#include <iostream>
#include <sys/socket.h>


int segreceiver::rec() {
  buffer.resize(1024);
  int sig = recv(_socket, buffer.data(), 1024, 0);
  std::cout<<"rec"<<sig<<endl;
  if (sig >= 1) {
    load.clear();
    msg = buffer[0];
    len = (static_cast<uint16_t>(buffer[2]) << 8) +
          static_cast<uint16_t>(buffer[3]);
    if (len > 0) {
      load.insert(load.end(), buffer.begin()+4,buffer.end());
    }
  }return sig;
}