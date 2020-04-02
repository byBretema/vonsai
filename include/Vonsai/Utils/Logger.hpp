#include <fmt/format.h>

#define vo_fmt(...) fmt::format(__VA_ARGS__);
#define vo_print(...) fmt::print(__VA_ARGS__);

#define vo_log(...)                                           \
  fmt::print("\n\nVONSAI_LOG {}:{}\n↪ ", __FILE__, __LINE__); \
  fmt::print(__VA_ARGS__);                                    \
  fmt::print("\n");

#define vo_warn(...)                                           \
  fmt::print("\n\nVONSAI_WARN {}:{}\n↪ ", __FILE__, __LINE__); \
  fmt::print(__VA_ARGS__);                                     \
  fmt::print("\n");

#define vo_err(...)                                           \
  fmt::print("\n\nVONSAI_ERR {}:{}\n↪ ", __FILE__, __LINE__); \
  fmt::print(__VA_ARGS__);                                    \
  fmt::print("\n");
