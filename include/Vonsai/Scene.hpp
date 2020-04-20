#pragma once

#include <functional>
#include <mutex>

#include <chrono>
using Clock = std::chrono::high_resolution_clock;


namespace Vonsai {

class Scene {
public:
  explicit Scene();

  unsigned int getFPS();
  float        getDeltaTime();

  void setOnGuiFn(std::function<void(void)> a_fn);
  void setOnUpdateFn(std::function<void(void)> a_fn);

  void setClearColor(float a_r, float a_g, float a_b);

private:
  std::function<void(void)>       m_onGui{nullptr}; // * User defined
  std::function<void(void)> const m_internalOnGui{[]() {}};

  std::function<void(void)>               m_onUpdate{nullptr}; // * User defined
  std::function<unsigned int(void)> const m_internalOnUpdate{[]() { return 0u; }};

  float             m_deltaTime{0.f};
  Clock::time_point m_deltaTimeStamp{Clock::now()};

  unsigned int      m_exposedFrameCounter{0u};
  unsigned int      m_internalFrameCounter{0u};
  Clock::time_point m_frameCounterTimeStamp{Clock::now()};

  void updateFPS();
  void updateDeltaTime();

  // Friends of this class:
  friend class SceneAttorney;
};

class SceneAttorney {
private:
  friend class Engine;
  static std::function<unsigned int(void)> const &getOnUpdateFn(Scene const &a_scene);
  static std::function<void(void)> const &        getOnGuiFn(Scene const &a_scene);
};

} // namespace Vonsai
