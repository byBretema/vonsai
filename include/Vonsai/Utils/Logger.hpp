#pragma once

#include <fmt/format.h>
#include <mutex>

static inline std::mutex coutMutex;

#define vo_fmt(...) fmt::format(__VA_ARGS__)


#ifndef VONSAI_HIDE_MSGS

#define vo_print(...)      \
  fmt::print(__VA_ARGS__); \
  fmt::print("\n");

#define vo_info(...)                                \
  {                                                 \
    std::lock_guard lockCout(coutMutex);            \
    fmt::print("[I] {}:{} | ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                        \
    fmt::print("\n");                               \
  }

#define vo_log(...)                                 \
  {                                                 \
    std::lock_guard lockCout(coutMutex);            \
    fmt::print("[L] {}:{} | ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                        \
    fmt::print("\n");                               \
  }

#define vo_warn(...)                                \
  {                                                 \
    std::lock_guard lockCout(coutMutex);            \
    fmt::print("[W] {}:{} | ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                        \
    fmt::print("\n");                               \
  }

#define vo_err(...)                                 \
  {                                                 \
    std::lock_guard lockCout(coutMutex);            \
    fmt::print("[E] {}:{} | ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                        \
    fmt::print("\n");                               \
  }

#else

#define vo_print(...)
#define vo_info(...)
#define vo_log(...)
#define vo_warn(...)
#define vo_err(...)

#endif // VONSAI_HIDE_MSGS


#ifndef VONSAI_HIDE_DEBUG_MSGS
#define vo_debug(...)                               \
  {                                                 \
    std::lock_guard lockCout(coutMutex);            \
    fmt::print("[D] {}:{} | ", __FILE__, __LINE__); \
    fmt::print(__VA_ARGS__);                        \
    fmt::print("\n");                               \
  }
#else
#define vo_debug(...)
#endif // VONSAI_HIDE_DEBUG_MSGS
