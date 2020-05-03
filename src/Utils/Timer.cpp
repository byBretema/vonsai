#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Utils/Timer.hpp>


namespace Vonsai {

Timer::Timer(std::string const &msg) : m_msg(msg), m_iTime(Clock::now()) {}
Timer::~Timer() {
  vo_print("[T] {:.{}f} s | {}", std::chrono::duration<double>(Clock::now() - m_iTime).count(), 4, m_msg);
}

} // namespace Vonsai
