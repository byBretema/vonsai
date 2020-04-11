#include <Vonsai/Camera.hpp>

#include <Vonsai/Utils/Logger.hpp>

namespace Vonsai {

void Camera::setZoom(float a_variation) { m_offset.z += a_variation; }
void Camera::setFOV(float a_variation) {
  m_fovY += glm::radians(a_variation);
  m_fovY = glm::clamp(m_fovY, MIN_FOV, MAX_FOV);
}

void Camera::frame(float a_aspectRatio, glm::vec3 const &a_target) {
  if (a_target != INF3) { pivot.setPos(a_target); }

  vo_print("camera rot = {}\n", glm::to_string(pivot.getRot()));

  if (pivot.getRotZ() != 0.f) { pivot.setRotZ(0.f); }                                 // ! Avoid any ROLL
  if (pivot.getRotX() >= 90.f and pivot.getRotX() < 180.f) { pivot.setRotX(89.9f); }  // * Avoid camera flip at 90º
  if (pivot.getRotX() >= 180.f and pivot.getRotX() < 270.f) { pivot.setRotX(270.f); } // * Avoid camera flip at 270º
  auto const [F, R, U] = pivot.getVectors();

  // Movement
  //----------
  float const speed = 0.1f;
  if (movement.F) { pivot.modPos(F * speed); }
  if (movement.B) { pivot.modPos(-F * speed); }
  if (movement.R) { pivot.modPos(R * speed); }
  if (movement.L) { pivot.modPos(-R * speed); }
  if (movement.U) { pivot.modPos(U * speed); }
  if (movement.D) { pivot.modPos(-U * speed); }

  // Matrices
  //----------
  glm::vec3 const eye = pivot.getPos() - (F * m_offset.z);
  m_view              = glm::lookAt(eye, eye + F, worldUp);
  m_proj              = glm::perspective(m_fovY, a_aspectRatio, 0.1f, 1000.f);
  m_viewproj          = m_proj * m_view;
}

void Camera::info() {
  vo_print("\nCAMERA\n  Fovy: {}\n  Offset: {},{},{}\n", m_fovY, m_offset.x, ",", m_offset.y, ",", m_offset.z);
}

} // namespace Vonsai
