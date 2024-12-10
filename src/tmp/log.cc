#define SPDLOG_USE_STD_FORMAT
#include <spdlog/spdlog.h>

int main() {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%@] %v");

  spdlog::info("Hello, World!");
  spdlog::debug("Hello, World!");

  SPDLOG_INFO("Hello, World!");
  SPDLOG_DEBUG("Hello, World!");

  return 0;
}