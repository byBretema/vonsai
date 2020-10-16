#include <Vonsai/Camera.hpp>

#include <Vonsai/IO/Input.hpp>
#include <Vonsai/UBO.hpp>

#include <Vonsai/Utils/Logger.hpp>

#define VO_CAMERA_ORBITAL

namespace Vonsai {

void Camera::setZoom(float aVariation) { m_offset.z += aVariation; }

void Camera::setFOV(float aVariation) {
  m_fovY += glm::radians(aVariation);
  m_fovY = glm::clamp(m_fovY, MIN_FOV, MAX_FOV);
}

void Camera::frame(float aAspectRatio, float aSpeed, bool aOrbital, glm::vec3 const &aTarget) {
  if (aTarget != INF3) { pivot.setPos(aTarget); }

  if (pivot.getRotZ() != 0.f) { pivot.setRotZ(0.f); }                                 // ! Avoid any ROLL
  if (pivot.getRotX() >= 90.f and pivot.getRotX() < 180.f) { pivot.setRotX(89.9f); }  // * Avoid camera flip at 90º
  if (pivot.getRotX() >= 180.f and pivot.getRotX() < 270.f) { pivot.setRotX(270.f); } // * Avoid camera flip at 270º
  auto const [F, R, U] = pivot.getVectors();

  // Movement
  //----------
  if (aOrbital) {
    if (movement.F) { m_offset.z -= aSpeed; }
    if (movement.B) { m_offset.z += aSpeed; }
    if (movement.R) { m_offset.x += aSpeed; }
    if (movement.L) { m_offset.x -= aSpeed; }
    if (movement.U) { m_offset.y -= aSpeed; }
    if (movement.D) { m_offset.y += aSpeed; }
  } else { // * FPS
    if (movement.F) { pivot.modPos(F * aSpeed); }
    if (movement.B) { pivot.modPos(-F * aSpeed); }
    if (movement.R) { pivot.modPos(R * aSpeed); }
    if (movement.L) { pivot.modPos(-R * aSpeed); }
    if (movement.U) { pivot.modPos(-U * aSpeed); }
    if (movement.D) { pivot.modPos(U * aSpeed); }
  }

  // Matrices
  //----------
  glm::vec3 const eye = pivot.getPos() + (m_offset.x * R) + (m_offset.y * U) - (m_offset.z * F);
  m_view              = glm::lookAt(eye, eye + F, UP);
  m_proj              = glm::perspective(m_fovY, aAspectRatio, 0.1f, 1'000'000.f);
  // m_proj              = glm::perspective(m_fovY, aAspectRatio, 0.0001f, 1'000'000.f);
  m_viewproj = m_proj * m_view;
}

void Camera::defaultBehaviour(float aDeltaTime, float aAspectRatio, UBO &aUBO, Input const &aIO) {
  float DT = aDeltaTime * 2.f;

  // Camera update
  frame(aAspectRatio, DT);

  // UBO update
  aUBO.setData("u_proj", getProj());
  aUBO.setData("u_view", getView());

  // Camera movement
  if (aIO.keyHold(KeyCode::Q)) { movement.U = true; } // Up
  if (!aIO.keyHold(KeyCode::Q)) { movement.U = false; }
  if (aIO.keyHold(KeyCode::E)) { movement.D = true; } // Down
  if (!aIO.keyHold(KeyCode::E)) { movement.D = false; }
  if (aIO.keyHold(KeyCode::W)) { movement.F = true; } // Front
  if (!aIO.keyHold(KeyCode::W)) { movement.F = false; }
  if (aIO.keyHold(KeyCode::S)) { movement.B = true; } // Back
  if (!aIO.keyHold(KeyCode::S)) { movement.B = false; }
  if (aIO.keyHold(KeyCode::D)) { movement.R = true; } // Right
  if (!aIO.keyHold(KeyCode::D)) { movement.R = false; }
  if (aIO.keyHold(KeyCode::A)) { movement.L = true; } // Left
  if (!aIO.keyHold(KeyCode::A)) { movement.L = false; }
  if (aIO.keyHold(KeyCode::Num0)) { pivot.reset(); } // Reset

  // Scroll for Fov / Shift + Scroll for Zoom
#ifdef __APPLE__
  (aIO.anySuperHold())
#else
  (aIO.anyCtrlHold())
#endif
      ? (aIO.keyHold(KeyCode::LeftShift)) ? setZoom(aIO.scrollV() * 10.f) : setFOV(aIO.scrollV())
      : (aIO.keyHold(KeyCode::LeftShift)) ? setZoom(aIO.scrollV()) : setFOV(aIO.scrollV());

  // Cmd/Ctrl: to rotate camera
  if (aIO.clickHoldM() or
#ifdef __APPLE__
      (aIO.anySuperHold() and aIO.clickHoldL())
#else
      (aIO.anyCtrlHold() and aIO.clickHoldL())
#endif
  ) {
    pivot.modRot(glm::vec3{aIO.axisV(), aIO.axisH(), 0.f} * 0.25f);
  }
}

glm::mat4 Camera::getView() const { return m_view; }
glm::mat4 Camera::getProj() const { return m_proj; }
glm::mat4 Camera::getViewProj() const { return m_viewproj; }

void Camera::info() {
  vo_print("\nCAMERA\n  Fovy: {}\n  Offset: {},{},{}\n", m_fovY, m_offset.x, ",", m_offset.y, ",", m_offset.z);
}

} // namespace Vonsai
