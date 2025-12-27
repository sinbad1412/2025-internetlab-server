#pragma once
#include <cstdint>
#include <string>
#include <vector>
using namespace std;
struct segment_header {
  static constexpr size_t LENGTH = 4;

  uint8_t msg=0;
  uint8_t flag=0;
  uint16_t length = 0;

  void parse(string p);
  std::vector<uint8_t> serialize() const;

};