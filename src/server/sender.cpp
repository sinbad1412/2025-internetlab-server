#include "server/sender.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
#include "helper/gettime.hpp"
vector<uint8_t> segsender::_send( vector<uint8_t> load, uint8_t msg) {
  segment seg;
  seg.header().msg = msg;

  if (msg == 0x82) {
    uint16_t len = load.size();
    load.insert(load.begin() , static_cast<uint8_t>((len >> 8) & 0xFF));
    load.insert(load.begin() +1, static_cast<uint8_t>((len ) & 0xFF));
  }
  if (msg == 0x81) {
    uint64_t time = _get_current_time();
    for (int i = 0; i < 8; i++) {
      load.insert(load.end(),static_cast<uint8_t>((time>>(56-i*8))&0xFF));
    }
  }
  if (msg == 0x83) {
  }
  if (msg == 0x84) {
    
  }
  seg.payload() = load;
  seg.header().length = load.size();
  vector<uint8_t> r = seg.serialize();
  // for (auto c: r) {
  //   std::cout << int(c) << std::endl;
  // }
  send(_socket, r.data(), r.size(), 0);
  
  return r;
}
vector<uint8_t> segsender::stringtoint(string s) {
  vector<uint8_t> result;
  for (auto c : s) {
    uint8_t a = c;
    result.push_back(a);
  }return result;
}