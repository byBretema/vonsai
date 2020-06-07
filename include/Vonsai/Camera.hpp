#pragma once

#include "Transform.hpp"
#include "Wraps/_glm.hpp"

namespace Vonsai {
class UBO;        // ! Forward decl of UBO
class Input;      // ! Forward decl of Input
class Renderable; // ! Forward decl of Renderable


class Camera {
public:
  static constexpr float MIN_FOV = 0.1f;
  static constexpr float MAX_FOV = 1.5f;

  Transform pivot;

  struct {
    bool F{false}, B{false}, R{false}, L{false}, U{false}, D{false};
  } movement;

  void setZoom(float a_variation);
  void setFOV(float a_variation);
  void frame(float a_aspectRatio, float speed = 0.5f, bool orbital = true, glm::vec3 const &a_target = INF3);

  // return a tuple {modelView, normalMat}
  std::tuple<glm::mat4, glm::mat4> genModelMatrices(Renderable const &a_r) const;

  void defaultBehaviour(float deltaTime, float a_aspectRatio, UBO &a_ubo, Input const &a_io);

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
