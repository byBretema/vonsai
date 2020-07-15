#include <Vonsai/Camera.hpp>

#include <Vonsai/IO/Input.hpp>
#include <Vonsai/UBO.hpp>

#include <Vonsai/Utils/Logger.hpp>

#define VO_CAMERA_ORBITAL

namespace Vonsai {

void Camera::setZoom(float a_variation) { m_offset.z += a_variation; }
void Camera::setFOV(float a_variation) {
  m_fovY += glm::radians(a_variation);
  m_fovY = glm::clamp(m_fovY, MIN_FOV, MAX_FOV);
}

void Camera::frame(float a_aspectRatio, float a_speed, bool a_orbital, glm::vec3 const &a_target) {
  if (a_target != INF3) { pivot.setPos(a_target); }

  if (pivot.getRotZ() != 0.f) { pivot.setRotZ(0.f); }                                 // ! Avoid any ROLL
  if (pivot.getRotX() >= 90.f and pivot.getRotX() < 180.f) { pivot.setRotX(89.9f); }  // * Avoid camera flip at 90º
  if (pivot.getRotX() >= 180.f and pivot.getRotX() < 270.f) { pivot.setRotX(270.f); } // * Avoid camera flip at 270º
  auto const [F, R, U] = pivot.getVectors();

  // Movement
  //----------
  if (a_orbital) {
    if (movement.F) { m_offset.z -= a_speed; }
    if (movement.B) { m_offset.z += a_speed; }
    if (movement.R) { m_offset.x += a_speed; }
    if (movement.L) { m_offset.x -= a_speed; }
    if (movement.U) { m_offset.y -= a_speed; }
    if (movement.D) { m_offset.y += a_speed; }
  } else { // * FPS
    if (movement.F) { pivot.modPos(F * a_speed); }
    if (movement.B) { pivot.modPos(-F * a_speed); }
    if (movement.R) { pivot.modPos(R * a_speed); }
    if (movement.L) { pivot.modPos(-R * a_speed); }
    if (movement.U) { pivot.modPos(-U * a_speed); }
    if (movement.D) { pivot.modPos(U * a_speed); }
  }

  // Matrices
  //----------
  glm::vec3 const eye = pivot.getPos() + (m_offset.x * R) + (m_offset.y * U) - (m_offset.z * F);
  m_view              = glm::lookAt(eye, eye + F, UP);
  m_proj              = glm::perspective(m_fovY, a_aspectRatio, 0.0001f, 1000.f);
  m_viewproj          = m_proj * m_view;
}

void Camera::defaultBehaviour(float deltaTime, float a_aspectRatio, UBO &a_ubo, Input const &a_io) {
  float DT = deltaTime * 2.f;

  // Camera update
  frame(a_aspectRatio, DT);

  // UBO update
  a_ubo.setData("u_proj", getProj());
  a_ubo.setData("u_view", getView());

  // Camera movement
  if (a_io.keyHold(KeyCode::Q)) { movement.U = true; } // Up
  if (!a_io.keyHold(KeyCode::Q)) { movement.U = false; }
  if (a_io.keyHold(KeyCode::E)) { movement.D = true; } // Down
  if (!a_io.keyHold(KeyCode::E)) { movement.D = false; }
  if (a_io.keyHold(KeyCode::W)) { movement.F = true; } // Front
  if (!a_io.keyHold(KeyCode::W)) { movement.F = false; }
  if (a_io.keyHold(KeyCode::S)) { movement.B = true; } // Back
  if (!a_io.keyHold(KeyCode::S)) { movement.B = false; }
  if (a_io.keyHold(KeyCode::D)) { movement.R = true; } // Right
  if (!a_io.keyHold(KeyCode::D)) { movement.R = false; }
  if (a_io.keyHold(KeyCode::A)) { movement.L = true; } // Left
  if (!a_io.keyHold(KeyCode::A)) { movement.L = false; }
  if (a_io.keyHold(KeyCode::Num0)) { pivot.reset(); } // Reset

  // Scroll for Fov / Shift + Scroll for Zoom
  (a_io.keyHold(KeyCode::LeftShift)) ? setZoom(a_io.scrollV()) : setFOV(a_io.scrollV());

  // Cmd/Ctrl: to rotate camera
  if (a_io.clickHoldL()) { pivot.modRot(glm::vec3{a_io.axisV(), a_io.axisH(), 0.f} * 0.25f); }
}

glm::mat4 Camera::getView() const { return m_view; }
glm::mat4 Camera::getProj() const { return m_proj; }
glm::mat4 Camera::getViewProj() const { return m_viewproj; }

void Camera::info() {
  vo_print("\nCAMERA\n  Fovy: {}\n  Offset: {},{},{}\n", m_fovY, m_offset.x, ",", m_offset.y, ",", m_offset.z);
}

} // namespace Vonsai
