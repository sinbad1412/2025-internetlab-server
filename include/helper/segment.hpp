#pragma once
#include "segment_header.hpp"
#include <cstdint>
#include <string>
#include <vector>
using namespace std;
class segment {
private:
  segment_header _header{};
  std::vector<uint8_t> _payload{};

public:
  const segment_header &header() const{ return _header; }
  const vector<uint8_t> &payload() const{ return _payload; }
  segment_header &header()  { return _header; }
  vector<uint8_t> &payload()  { return _payload; }
  vector<uint8_t> serialize() const;
  
};
