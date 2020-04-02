#include <Vonsai/Transform.hpp>

#include <Vonsai/Utils/Logger.hpp>

namespace Vonsai {

glm::mat4 Transform::matrix() const {
  glm::mat4 aux{1.f};
  aux = glm::translate(aux, pos);
  aux = glm::rotate(aux, glm::radians(rot.z), worldFront);
  aux = glm::rotate(aux, glm::radians(rot.y), worldUp);
  aux = glm::rotate(aux, glm::radians(rot.x), worldRight);
  aux = glm::scale(aux, scl);
  return aux;
}

std::tuple<glm::vec3, glm::vec3, glm::vec3> Transform::getVectors() const {
  glm::vec3 front = normalize(matrix()[2]).xyz();
  front.z *= -1.f;
  glm::vec3 right = normalize(cross(front, worldUp));
  glm::vec3 up    = normalize(cross(front, right));

  return {front, right, up};
}

void Transform::reset() {
  pos = glm::vec3{0.f};
  scl = glm::vec3{1.f};
  rot = glm::vec3{0.f};
}

void Transform::info() {
  vo_print("\nTRANSFORM\n  Pos: {},{},{}\n  Scl: {},{},{}\n  Rot: {},{},{}\n", //
           pos.x, pos.y, pos.z,                                                //
           scl.x, scl.y, scl.z,                                                //
           rot.x, rot.y, rot.z                                                 //
  );
}

} // namespace Vonsai
