#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Vonsai {

glm::vec3 constexpr MUTE_X = {0, 1, 1};
glm::vec3 constexpr MUTE_Y = {1, 0, 1};
glm::vec3 constexpr MUTE_Z = {1, 1, 0};

glm::vec3 constexpr MUTE_YZ = {1, 0, 0};
glm::vec3 constexpr MUTE_XZ = {0, 1, 0};
glm::vec3 constexpr MUTE_XY = {0, 0, 1};

glm::vec3 constexpr RIGHT = {1, 0, 0};
glm::vec3 constexpr UP    = {0, 1, 0};
glm::vec3 constexpr FRONT = {0, 0, 1};

float constexpr INF      = std::numeric_limits<float>::infinity();
glm::vec3 constexpr INF3 = {INF, INF, INF};

} // namespace Vonsai
