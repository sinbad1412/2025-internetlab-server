#include "server/sender.hpp"
#include <cstdint>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
#include "helper/gettime.hpp"
vector<uint8_t> segsender::_send( string load, uint8_t msg) {
  segment seg;
  seg.header().msg = msg;
 
  vector<uint8_t> _load;
  for (auto c : load) {
    uint8_t a = c;
    _load.push_back(a);
  }
  
  if (msg == 0x82) {
    uint16_t len = load.size();
    _load.insert(_load.begin() , static_cast<uint8_t>((len >> 8) & 0xFF));
    _load.insert(_load.begin() +1, static_cast<uint8_t>((len ) & 0xFF));
  }
  if (msg == 0x81) {
    uint64_t time = _get_current_time();
    for (int i = 0; i < 8; i++) {
      _load.insert(_load.end(),static_cast<uint8_t>((time>>(56-i*8))&0xFF));
    }
  }
  if (msg == 0x83) {
  }
  if (msg == 0x84) {
  }
  seg.payload() = _load;
  seg.header().length = _load.size();
  vector<uint8_t> r = seg.serialize();
  // for (auto c: r) {
  //   std::cout << int(c) << std::endl;
  // }
  send(_socket, r.data(), r.size(), 0);
  
  return r;
}