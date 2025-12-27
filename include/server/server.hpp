#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <vector>
#include "sender.hpp"
#include "receiver.hpp"

namespace server {

// 简单的日志工具函数
namespace logger {
void info(const std::string &message);
void warning(const std::string &message);
void error(const std::string &message);
void log(const std::string &level, const std::string &message);
} // namespace logger

// 网络工具函数
namespace net_utils {
std::string ip_to_string(const sockaddr_in &addr);
std::string get_client_info(const sockaddr_in &client_addr);
} // namespace net_utils

class TcpServer {
public:
  struct Config {
    uint16_t port = 3024;
    int max_queue_size = 20;
    std::string bind_address = "0.0.0.0";
  };

  explicit TcpServer(const Config &config);
  ~TcpServer();

  // 禁用拷贝
  TcpServer(const TcpServer &) = delete;
  TcpServer &operator=(const TcpServer &) = delete;

  // 允许移动
  TcpServer(TcpServer &&) noexcept;
  TcpServer &operator=(TcpServer &&) noexcept;

  bool start();
  void stop();
  bool is_running() const { return running_; }
  
private:
  std::map<int, sockaddr_in> socket_table{};
  Config config_;
  int server_socket_ = -1;
  bool running_ = false;
  std::vector<std::thread> client_threads_;

  void cleanup();
  void handle_client(int client_socket, const sockaddr_in &client_addr);
  static void client_thread_proc(TcpServer *server, int client_socket,
                                 sockaddr_in client_addr);
};

} // namespace server