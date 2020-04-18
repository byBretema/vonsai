#pragma once

#include <functional>

#include <chrono>
using Clock = std::chrono::high_resolution_clock;

namespace Vonsai {

class Scene {
public:
  std::function<void(void)> onGui{[]() {}};
  std::function<void(void)> onUpdate{[]() {}};

  explicit Scene() = default;

  uint16_t getFPS();
  float    getDeltaTime();

  void setClearColor(float r, float g, float b);

private:
  float             m_deltaTime{0.f};
  Clock::time_point m_deltaTimeStamp{Clock::now()};

  uint16_t          m_exposedFrameCounter{0u};
  uint16_t          m_internalFrameCounter{0u};
  Clock::time_point m_frameCounterTimeStamp{Clock::now()};

  void updateFPS();
  void updateDeltaTime();

  // TODO : Improve friend mechanism
  // * ( https://en.wikibooks.org/wiki/More_C++_Idioms/Friendship_and_the_Attorney-Client )
  friend class IO;

  void     internalOnGui();
  uint16_t internalOnUpdate();
};

} // namespace Vonsai
