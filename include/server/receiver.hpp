#include <cstdint>
#include <vector>
#include "helper/segment.hpp"
using namespace std;

class segreceiver {
private:
  std::vector<uint8_t>buffer ;
  uint8_t msg{0};
  uint16_t len{0};
  int _socket{-1};
  std::vector<uint8_t> load; //存储转发信息
  uint32_t tempsocket;
public:
  int rec();
  const uint8_t &get_msg() const { return msg; }
  void read_socket(int s) { _socket = s; }
  const uint32_t &getsocket() const { return tempsocket; }
  const vector<uint8_t> &getload() const { return load; }
  const vector<uint8_t> &getbuffer() const {return buffer;}
  void clearload() { load.clear(); }
  void readbuffer();
};
