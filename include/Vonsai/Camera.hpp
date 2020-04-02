#pragma once
#include "Transform.hpp"
#include "Wraps/_glm.hpp"

namespace Vonsai {

class Camera {
private:
  glm::mat4 m_view{1.f};
  glm::mat4 m_proj{1.f};
  glm::mat4 m_viewproj{1.f};

  float     m_fovY{0.4f};
  glm::vec3 m_offset{1.f};

public:
  Transform pivot;

  struct {
    bool F{false}, B{false}, R{false}, L{false}, U{false}, D{false};
  } movement;

  void zoom(float a_variation);
  void frame(float a_aspectRatio, glm::vec3 const &a_target = INF3);

  inline glm::mat4 getView() { return m_view; }
  inline glm::mat4 getProj() { return m_proj; }
  inline glm::mat4 getViewProj() { return m_viewproj; }

  void info();
};

} // namespace Vonsai
