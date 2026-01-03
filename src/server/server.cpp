#include "server/server.hpp"
#include "server/logger.hpp" // 添加这个
#include "server/sender.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <vector>

namespace server {

#define name "浙江大学计算机网络实验服务端3024 "

// net_utils实现要放在前面
namespace net_utils {

std::string ip_to_string(const sockaddr_in &addr) {
  char ip_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(addr.sin_addr), ip_str, INET_ADDRSTRLEN);
  return std::string(ip_str);
}

std::string get_client_info(const sockaddr_in &client_addr) {
  std::string ip = ip_to_string(client_addr);
  uint16_t port = ntohs(client_addr.sin_port);
  return ip + ":" + std::to_string(port);
}

} // namespace net_utils

TcpServer::TcpServer(const Config &config) : config_(config) {
  if (config_.port == 0) {
    throw std::invalid_argument("Port cannot be 0");
  }
}

TcpServer::~TcpServer() {
  stop();
  cleanup();
}

TcpServer::TcpServer(TcpServer &&other) noexcept
    : config_(std::move(other.config_)), server_socket_(other.server_socket_),
      running_(other.running_),
      client_threads_(std::move(other.client_threads_)) {
  other.server_socket_ = -1;
  other.running_ = false;
}

TcpServer &TcpServer::operator=(TcpServer &&other) noexcept {
  if (this != &other) {
    stop();
    cleanup();

    config_ = std::move(other.config_);
    server_socket_ = other.server_socket_;
    running_ = other.running_;
    client_threads_ = std::move(other.client_threads_);

    other.server_socket_ = -1;
    other.running_ = false;
  }
  return *this;
}

bool TcpServer::start() {
  if (running_) {
    logger::warning("服务器已经在运行");
    return false;
  }

  // 创建socket
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ < 0) {
    logger::error("创建socket失败: " + std::string(strerror(errno)));
    return false;
  }

  // 设置SO_REUSEADDR选项
  int opt = 1;
  if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {
    logger::warning("Failed to set SO_REUSEADDR: " +
                    std::string(strerror(errno)));
  }

  // 绑定地址
  sockaddr_in server_addr{};
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(config_.port);

  if (config_.bind_address == "0.0.0.0") {
    server_addr.sin_addr.s_addr = INADDR_ANY;
  } else {
    if (inet_pton(AF_INET, config_.bind_address.c_str(),
                  &server_addr.sin_addr) <= 0) {
      logger::error("无效地址: " + config_.bind_address);
      close(server_socket_);
      server_socket_ = -1;
      return false;
    }
  }

  if (bind(server_socket_, reinterpret_cast<struct sockaddr *>(&server_addr),
           sizeof(server_addr)) < 0) {
    logger::error("端口配对失败： " + std::to_string(config_.port) +
                  ": " + std::string(strerror(errno)));
    close(server_socket_);
    server_socket_ = -1;
    return false;
  }

  // 开始监听
  if (listen(server_socket_, config_.max_queue_size) < 0) {
    logger::error("监听失败: " + std::string(strerror(errno)));
    close(server_socket_);
    server_socket_ = -1;
    return false;
  }

  running_ = true;
  logger::info("服务开始在端口 " + std::to_string(config_.port) +
               " (bind address: " + config_.bind_address + ")");

  // 接受客户端连接
  while (running_) {
    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);

    int client_socket = accept(
        server_socket_, reinterpret_cast<struct sockaddr *>(&client_addr),
        &client_addr_len);

    if (client_socket < 0) {
      if (running_) {
        logger::error("连接失败: " + std::string(strerror(errno)));
      }
      continue;
    }

    // 记录客户端连接
    logger::info("客户端已连接: " +
                 net_utils::get_client_info(client_addr));

    // 创建线程处理客户端
    client_threads_.emplace_back(client_thread_proc, this, client_socket,
                                 client_addr);
    socket_table.insert({client_socket, client_addr});

    // for (auto t = client_threads_.begin(); t != client_threads_.end(); ) {
    //   if (t->joinable()) {
    //     t->join();
    //     t = client_threads_.erase(t);
    //   } else {
    //     t++;
    //   }
    // }
  }
  return true;
}

void TcpServer::stop() {
  if (!running_) {
    return;
  }

  running_ = false;

  // 关闭服务器socket以中断accept调用
  if (server_socket_ >= 0) {
    shutdown(server_socket_, SHUT_RDWR);
    close(server_socket_);
    server_socket_ = -1;
  }

  // 等待所有客户端线程结束
  for (auto &thread : client_threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  client_threads_.clear();

  logger::info("服务端停止");
}

void TcpServer::cleanup() {
  if (server_socket_ >= 0) {
    close(server_socket_);
    server_socket_ = -1;
  }
}

void TcpServer::handle_client(int client_socket,
                              const sockaddr_in &client_addr) {
  const std::string client_info = net_utils::get_client_info(client_addr);
  segsender sender;
  segreceiver receiver;
  receiver.read_socket(client_socket);
  sender.read_socket(client_socket);
  try {
    // 发送欢迎消息
    const std::string message = "你好，这里是服务器，你已经成功连接";
    vector<uint8_t> mes = sender.stringtoint(message);
    sender._send(mes, 0x82);
    mes.clear();
    sender._send(mes, 0x81);
    int time_count=0;
    while (true) {
      // logger::info("communicating");
      mes.clear();
      int sig = receiver.rec();
      if (sig <= 0 && sig != EWOULDBLOCK) {
        break;
      }
      while (!receiver.getbuffer().empty()) {
        receiver.readbuffer();
        // 读取缓存区内的一条消息
        if (receiver.get_msg() == 0) {
          break;
        }
        logger::info("收到一条客户端请求，类型为：" + std::to_string(receiver.get_msg()));
        
        if (receiver.get_msg() == 0x01) {
          mes.clear();
          sender._send(mes, 0x81);
          time_count += 1;
         // std::cout<<"time requset recv:"<<time_count<<endl;
      } else if (receiver.get_msg() == 0x02) {
          mes = sender.stringtoint(name);                                              
          sender._send(mes, 0x82);
      } else if (receiver.get_msg() == 0x03) {
          mes.clear();
          uint16_t len = socket_table.size();
          mes.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
          mes.push_back(static_cast<uint8_t>((len) & 0xFF));
          for (auto it = socket_table.begin(); it != socket_table.end(); it++) {
            uint32_t id = static_cast<uint32_t>(it->first);
            uint32_t ip = it->second.sin_addr.s_addr;
            uint16_t port = it->second.sin_port;
            mes.push_back(static_cast<uint8_t>((id >> 24) & 0xFF));
            mes.push_back(static_cast<uint8_t>((id >> 16) & 0xFF));
            mes.push_back(static_cast<uint8_t>((id >> 8) & 0xFF));
            mes.push_back(static_cast<uint8_t>((id) & 0xFF));
            mes.push_back(static_cast<uint8_t>((ip >> 24) & 0xFF));
            mes.push_back(static_cast<uint8_t>((ip >> 16) & 0xFF));
            mes.push_back(static_cast<uint8_t>((ip >> 8) & 0xFF));
            mes.push_back(static_cast<uint8_t>((ip) & 0xFF));
            mes.push_back(static_cast<uint8_t>((port) & 0xFF));
            mes.push_back(static_cast<uint8_t>((port >> 8) & 0xFF));
          
        }
          sender._send(mes, 0x83);
      }   else if (receiver.get_msg() == 0x04) {
            sender.read_socket(receiver.getsocket());
            mes.push_back(static_cast<uint8_t>((client_socket >> 24) & 0xFF));
            mes.push_back(static_cast<uint8_t>((client_socket >> 16) & 0xFF));
            mes.push_back(static_cast<uint8_t>((client_socket >> 8) & 0xFF));
            mes.push_back(static_cast<uint8_t>((client_socket) & 0xFF));
            vector<uint8_t> load = receiver.getload();
            uint16_t len = load.size();
            mes.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
            mes.push_back(static_cast<uint8_t>((len) & 0xFF));
            mes.insert(mes.end(), load.begin(), load.end());
            sender._send(mes, 0x84);
            sender.read_socket(client_socket); // 恢复原来的客户端

        } 
        }
    }

    // 这里可以添加更多的客户端处理逻辑
    // 例如：接收客户端数据、处理请求等

  } catch (const std::exception &e) {
    logger::error("处理该客户端时出错： " + client_info + ": " +
                  e.what());
  }

  // 关闭客户端连接
  socket_table.erase(client_socket);
  close(client_socket);
  logger::info("连接断开: " + client_info);
}

void TcpServer::client_thread_proc(TcpServer *server, int client_socket,
                                   sockaddr_in client_addr) {
  server->handle_client(client_socket, client_addr);
}

} // namespace server
