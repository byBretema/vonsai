#include <Vonsai/Utils/Colors.hpp>

#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Utils/Random.hpp>

namespace Vonsai::Colors {

inline glm::vec3 const FALLBACK_COLOR = glm::vec3{1.f, -0.123f, 1.f};

glm::vec3 random() { return {Random::Float01(), Random::Float01(), Random::Float01()}; }

glm::vec3 rgbFromHex(std::string const &a_hex) {
  std::string hex = a_hex.substr(1);

  if (hex.size() == 3) {
    std::string const R = hex.substr(0, 1);
    std::string const G = hex.substr(1, 1);
    std::string const B = hex.substr(2, 1);
    hex                 = R + R + G + G + B + B;
  }

  if (hex.size() == 6) {
    unsigned int const C = std::stoul(hex, nullptr, 16);
    unsigned int const B = C % 256;
    unsigned int const G = ((C - B) / 256) % 256;
    unsigned int const R = ((C - B) / (256 * 256)) - G / 256;

    return glm::vec3{R, G, B};
  }

  vo_err("Invalid HEX color");
  return FALLBACK_COLOR;
}

glm::vec3 glslFromHex(std::string const &a_hex) {
  glm::vec3 C = rgbFromHex(a_hex);
  return (C != FALLBACK_COLOR) ? glm::vec3{C.r / 255.f, C.g / 255.f, C.b / 255.f} : FALLBACK_COLOR;
}

} // namespace Vonsai::Colors
