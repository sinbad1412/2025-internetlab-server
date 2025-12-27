#include "helper/segment.hpp"
#include <cstdint>
#include <vector>


std::vector<uint8_t>segment::serialize() const {
  std::vector<uint8_t> result=header().serialize();
  for (auto c : payload()) {
    uint8_t a = c;
    result.push_back(c);
  }
  return result ;
}
