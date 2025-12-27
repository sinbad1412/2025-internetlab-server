// logger.hpp
#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace server {
namespace logger {

// 获取当前时间字符串
inline std::string get_current_time() {
  auto now = std::chrono::system_clock::now();
  auto now_time = std::chrono::system_clock::to_time_t(now);
  auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()) %
                1000;

  std::tm *tm = std::localtime(&now_time);
  char buffer[64];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);

  return std::string(buffer) + "." + std::to_string(now_ms.count());
}

// 日志函数
inline void log(const std::string &level, const std::string &message) {
  std::cerr << "[" << get_current_time() << "] " << "[" << level << "] "
            << message << std::endl;
}

inline void info(const std::string &message) { log("INFO", message); }

inline void warning(const std::string &message) { log("WARNING", message); }

inline void error(const std::string &message) { log("ERROR", message); }

} // namespace logger
} // namespace server