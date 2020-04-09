#pragma once

#include <functional>

namespace Vonsai {
using RenderFn = std::function<void(void)>;

class Scene {
public:
  static inline RenderFn const NullRenderFn{[]() {}};

  void        update();
  void        setClearColor(glm::vec3 const &a_ClearColor);
  inline void setRenderFn(RenderFn a_func) { m_onRender = (!a_func) ? s_nullRenderFn : a_func; }

private:
  RenderFn  m_onRender{s_nullRenderFn};
  glm::vec3 m_clearColor{1.f, 0.f, 1.f};

  unsigned int m_frames{0u};
  double       m_timeStamp{glfwGetTime()};

  double m_deltaTime{glfwGetTime()};

  void updateFPS();
};

} // namespace Vonsai
