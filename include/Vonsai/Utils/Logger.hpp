#pragma once

#include <fmt/format.h>
#include <mutex>

static inline std::mutex coutMutex;

#define vo_fmt(...) fmt::format(__VA_ARGS__)

#ifndef VONSAI_HIDE_DEBUG_MSGS
#define vo_debug(...)                                           \
  {                                                             \
    std::lock_guard lockCout(coutMutex);                        \
    fmt::print("\nVONSAI_DEBUG {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                    \
    fmt::print("\n");                                           \
  }
#else
#define vo_debug(...)
#endif

#ifndef VONSAI_HIDE_MSGS
#define vo_print(...)                                           \
  {                                                             \
    std::lock_guard lockCout(coutMutex);                        \
    fmt::print("\nVONSAI_PRINT {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                    \
    fmt::print("\n");                                           \
  }
#else
#define vo_print(...)
#endif

#ifndef VONSAI_HIDE_MSGS
#define vo_log(...)                                           \
  {                                                           \
    std::lock_guard lockCout(coutMutex);                      \
    fmt::print("\nVONSAI_LOG {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                  \
    fmt::print("\n");                                         \
  }
#else
#define vo_log(...)
#endif

#ifndef VONSAI_HIDE_MSGS
#define vo_warn(...)                                              \
  {                                                               \
    std::lock_guard lockCout(coutMutex);                          \
    fmt::print("\nVONSAI_WARNING {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                      \
    fmt::print("\n");                                             \
  }
#else
#define vo_warn(...)
#endif

#ifndef VONSAI_HIDE_MSGS
#define vo_err(...)                                             \
  {                                                             \
    std::lock_guard lockCout(coutMutex);                        \
    fmt::print("\nVONSAI_ERROR {}:{}\n↪ ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                                    \
    fmt::print("\n");                                           \
  }
#else
#define vo_err(...)
#endif
