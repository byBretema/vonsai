#include <Vonsai/Camera.hpp>

#include <Vonsai/Utils/Logger.hpp>

namespace Vonsai {

void Camera::zoom(float a_variation) {
  m_fovY += glm::radians(a_variation);
  // (Window::active()->getKey(GLFW_KEY_Z)) //
  //     ? offset.z += variation            //
  //     : fovY += glm::radians(variation); //
}

void Camera::frame(float a_aspectRatio, glm::vec3 const &a_target) {
  if (a_target != INF3) { pivot.pos = a_target; }
  auto const [F, R, U] = pivot.getVectors();

  // Movement
  //----------
  float const speed = 0.1f;
  if (movement.F) { pivot.pos += F * speed; }
  if (movement.B) { pivot.pos -= F * speed; }
  if (movement.R) { pivot.pos += R * speed; }
  if (movement.L) { pivot.pos -= R * speed; }
  if (movement.U) { pivot.pos += U * speed; }
  if (movement.D) { pivot.pos -= U * speed; }

  // Matrices
  //----------
  glm::vec3 const eye = pivot.pos - (F * m_offset.z);
  m_view              = glm::lookAt(eye, eye + F, worldUp);
  m_proj              = glm::perspective(m_fovY, a_aspectRatio, 0.1f, 1000.f);
  m_viewproj          = m_proj * m_view;
}

void Camera::info() {
  vo_print("\nCAMERA\n  Fovy: {}\n  Offset: {},{},{}\n", m_fovY, m_offset.x, ",", m_offset.y, ",", m_offset.z);
}

} // namespace Vonsai
