#pragma once

#include <chrono>
#include <string>
using Clock = std::chrono::high_resolution_clock;


namespace Vonsai {

class Timer {
public:
  Timer(std::string const &msg = "");
  ~Timer();

private:
  std::string       m_msg;
  Clock::time_point m_iTime;
};

} // namespace Vonsai
