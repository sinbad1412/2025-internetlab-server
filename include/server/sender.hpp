#pragma once

#include "helper/segment.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>


using namespace std;
class segsender {
private:
  int _socket{-1};
public:
  vector<uint8_t> _send(vector<uint8_t> load, uint8_t msg);
  void read_socket(int s) { _socket = s; }
  vector<uint8_t> stringtoint(string s);
};
