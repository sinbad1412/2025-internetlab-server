#include "helper/segment_header.hpp"
#include <cstdint>
#include <vector>

void segment_header::parse(string p) {}
std::vector<uint8_t> segment_header::serialize() const {
  std::vector<uint8_t> result;
  result.push_back(msg);
  result.push_back(flag);
  result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
  result.push_back(static_cast<uint8_t>(length & 0xFF));
  uint8_t *p = result.data();
  return  result;
}