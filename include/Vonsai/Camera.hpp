#pragma once

#include "Transform.hpp"
#include "Wraps/_glm.hpp"

namespace Vonsai {
class UBO;   // ! Forward decl of UBO
class Input; // ! Forward decl of Input


class Camera {
public:
  static constexpr float MIN_FOV = 0.1f;
  static constexpr float MAX_FOV = 1.5f;

  Camera() = default;
  Camera(float aZoom) { setZoom(aZoom); }

  Transform pivot;

  struct {
    bool F{false}, B{false}, R{false}, L{false}, U{false}, D{false};
  } movement;

  void setZoom(float aVariation);
  void setFOV(float aVariation);
  void frame(float aAspectRatio, float aSpeed = 0.5f, bool aOrbital = true, glm::vec3 const &aTarget = INF3);

  void defaultBehaviour(float aDeltaTime, float aAspectRatio, UBO &aUBO, Input const &aIO);

  glm::mat4 getView() const;
  glm::mat4 getProj() const;
  glm::mat4 getViewProj() const;

  void info();

private:
  glm::mat4 m_view{1.f};
  glm::mat4 m_proj{1.f};
  glm::mat4 m_viewproj{1.f};

  float     m_fovY{0.4f};
  glm::vec3 m_offset{0.f, 0.f, 1.f};
};

} // namespace Vonsai
