#pragma once

#include "Wraps/_glm.hpp"
#include <vector>

namespace Vonsai {

// TODO easy upload to shader

/**
 * This is the base implementation for
 * POINT Light, other light-types should
 * inherit from this.
*/
class Light {
public:
  glm::vec4 position{0.f, 0.f, 0.f, 1.f};
  glm::vec4 color{1.f, 0.f, 1.f, 1.f};
  glm::vec3 attenuation{0.f, 0.f, 0.f};
  float     intensity{1.f};

  Light(glm::vec3 const &a_pos, glm::vec3 const &a_color = {1.f, 0.f, 1.f})
      : position(a_pos, 1.f), color(a_color, 1.f) {}
};

} // namespace Vonsai
