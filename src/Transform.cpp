#include <Vonsai/Transform.hpp>
#include <Vonsai/Utils/Logger.hpp>

namespace Vonsai {

glm::mat4 Transform::matrix() const {
  glm::mat4 T = glm::translate(glm::mat4{1.f}, m_pos);

  glm::mat4 R{1.f};
  R = glm::rotate(R, glm::radians(m_rot.z), FRONT);
  R = glm::rotate(R, glm::radians(m_rot.y), UP);
  R = glm::rotate(R, glm::radians(m_rot.x), RIGHT);

  glm::mat4 S = glm::scale(glm::mat4{1.f}, m_scl);

  return T * R * S;
}

std::tuple<glm::vec3, glm::vec3, glm::vec3> Transform::getVectors() const {
  glm::vec3 front = normalize(matrix()[2]).xyz();
  front.z *= -1.f; // Just for OpenGL, disable for other APIs
  glm::vec3 right = normalize(cross(front, UP));
  glm::vec3 up    = normalize(cross(front, right));

  return {front, right, up};
}

void Transform::reset() {
  m_pos = glm::vec3{0.f};
  m_scl = glm::vec3{1.f};
  m_rot = glm::vec3{0.f};
}

void Transform::applyRotLimits() {
  auto clampRot = [](float angle) {
    auto turns = floorf(angle / 360.f);
    return angle - 360.f * turns;
  };
  m_rot.x = clampRot(m_rot.x);
  m_rot.y = clampRot(m_rot.y);
  m_rot.z = clampRot(m_rot.z);
}

void Transform::info() {
  vo_print("\nTRANSFORM\n  Pos: {},{},{}\n  Scl: {},{},{}\n  Rot: {},{},{}\n", //
           m_pos.x, m_pos.y, m_pos.z,                                          //
           m_scl.x, m_scl.y, m_scl.z,                                          //
           m_rot.x, m_rot.y, m_rot.z                                           //
  );
}

} // namespace Vonsai
