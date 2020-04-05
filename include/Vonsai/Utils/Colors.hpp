#pragma once

#include "../Wraps/_glm.hpp"

namespace Vonsai::Colors {

glm::vec3 const BLACK   = {0.0f, 0.0f, 0.0f};
glm::vec3 const WHITE   = {1.0f, 1.0f, 1.0f};
glm::vec3 const RED     = {1.0f, 0.0f, 0.0f};
glm::vec3 const MAGENTA = {1.0f, 0.0f, 1.0f};
glm::vec3 const PINK    = {1.0f, 0.0f, 0.5f};
glm::vec3 const ORANGE  = {1.0f, 0.5f, 0.0f};
glm::vec3 const GREEN   = {0.0f, 1.0f, 0.0f};
glm::vec3 const YELLOW  = {1.0f, 1.0f, 0.0f};
glm::vec3 const LIME    = {0.5f, 1.0f, 0.0f};
glm::vec3 const SLIME   = {0.0f, 1.0f, 0.5f};
glm::vec3 const BLUE    = {0.0f, 0.0f, 1.0f};
glm::vec3 const CYAN    = {0.0f, 1.0f, 1.0f};
glm::vec3 const VIOLET  = {0.5f, 0.0f, 1.0f};
glm::vec3 const TEAL    = {0.0f, 0.5f, 1.0f};

glm::vec3 random();
glm::vec3 rgbFromHEX(std::string const &a_hex);
glm::vec3 glslFromHEX(std::string const &a_hex);
glm::vec3 glslFromRGB(int a_R, int a_G, int a_B);
glm::vec3 glslFromRGB(glm::vec3 const &a_color);

} // namespace Vonsai::Colors
