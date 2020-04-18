#pragma once

#include <fmt/format.h>
#include <mutex>

static inline std::mutex coutMutex;

#define vo_fmt(...) fmt::format(__VA_ARGS__)

#define vo_print(...)                    \
  {                                      \
    std::lock_guard lockCout(coutMutex); \
    fmt::print(__VA_ARGS__);             \
  }

#define vo_log(...)                                             \
  {                                                             \
    std::lock_guard lockCout(coutMutex);                        \
    fmt::print("\n\nVONSAI_LOG {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                    \
  }

#define vo_warn(...)                                             \
  {                                                              \
    std::lock_guard lockCout(coutMutex);                         \
    fmt::print("\n\nVONSAI_WARN {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                     \
  }

#define vo_err(...)                                             \
  {                                                             \
    std::lock_guard lockCout(coutMutex);                        \
    fmt::print("\n\nVONSAI_ERR {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                    \
  }
