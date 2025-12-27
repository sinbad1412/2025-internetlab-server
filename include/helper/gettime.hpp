
#include <cstdint>
#include <chrono>
#include <cstdint>
uint64_t get_current_time() {
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);

  std::tm local_time;
#ifdef _WIN32
  localtime_s(&local_time, &time_t_now);
#else
  localtime_r(&time_t_now, &local_time);
#endif

  // 格式: YYYYMMDDHHMMSS (14位数字)
  uint64_t timestamp =
      (uint64_t)(local_time.tm_year + 1900) * 10000000000ULL + // 年份 * 10^10
      (uint64_t)(local_time.tm_mon + 1) * 100000000ULL +       // 月份 * 10^8
      (uint64_t)(local_time.tm_mday) * 1000000ULL +            // 日 * 10^6
      (uint64_t)(local_time.tm_hour) * 10000ULL +              // 时 * 10^4
      (uint64_t)(local_time.tm_min) * 100ULL +                 // 分 * 10^2
      (uint64_t)(local_time.tm_sec);                           // 秒

  return timestamp;
}
uint64_t _get_current_time() {
  std::time_t raw_time = std::time(nullptr);

  // 转为 uint64_t
  return static_cast<uint64_t>(raw_time);
}
