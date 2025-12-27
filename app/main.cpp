#include "server/server.hpp"
#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>

std::atomic<bool> g_running{true};

void signal_handler(int signal) {
  server::logger::info("Received signal " + std::to_string(signal) +
                       ", shutting down...");
  g_running = false;
}

int main(int argc, char *argv[]) {
  // 设置信号处理器
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  server::logger::info("Starting TCP server...");

  try {
    // 配置服务器
    server::TcpServer::Config config;
    config.port = 3024;
    config.max_queue_size = 20;
    config.bind_address = "0.0.0.0";

    // 创建并启动服务器
    server::TcpServer server(config);

    // 在后台运行服务器
    std::thread server_thread([&server]() {
      if (!server.start()) {
        server::logger::error("Failed to start server");
        g_running = false;
      }
    });

    // 主线程等待退出信号
    while (g_running) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 停止服务器
    server::logger::info("Stopping server...");
    server.stop();

    if (server_thread.joinable()) {
      server_thread.join();
    }

    server::logger::info("Server shutdown completed");

  } catch (const std::exception &e) {
    server::logger::error("Fatal error: " + std::string(e.what()));
    return 1;
  }

  return 0;
}