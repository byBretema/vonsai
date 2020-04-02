#pragma once
#include "Wraps/_glm.hpp"

namespace Vonsai {

struct Transform {
  glm::vec3 pos{0.f}, scl{1.f}, rot{0.f};

  glm::mat4                                   matrix() const;
  std::tuple<glm::vec3, glm::vec3, glm::vec3> getVectors() const;

  void reset();
  void info();
};

} // namespace Vonsai
