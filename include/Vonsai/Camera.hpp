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
  static constexpr float MIN_FOV = 0.1f;
  static constexpr float MAX_FOV = 1.5f;

  Transform pivot;

  struct {
    bool F{false}, B{false}, R{false}, L{false}, U{false}, D{false};
  } movement;

  void setZoom(float a_variation);
  void setFOV(float a_variation);
  void frame(float a_aspectRatio, glm::vec3 const &a_target = INF3);

  inline glm::mat4 const &getView() const { return m_view; }
  inline glm::mat4 const &getProj() const { return m_proj; }
  inline glm::mat4 const &getViewProj() const { return m_viewproj; }

  void info();
};

} // namespace Vonsai
