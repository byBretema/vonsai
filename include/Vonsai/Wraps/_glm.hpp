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

float constexpr INF = std::numeric_limits<float>::infinity();

glm::vec3 const muteX = {0, 1, 1};
glm::vec3 const muteY = {1, 0, 1};
glm::vec3 const muteZ = {1, 1, 0};

glm::vec3 const muteYZ = {1, 0, 0};
glm::vec3 const muteXZ = {0, 1, 0};
glm::vec3 const muteXY = {0, 0, 1};

glm::vec3 const worldRight = {1, 0, 0};
glm::vec3 const worldUp    = {0, 1, 0};
glm::vec3 const worldFront = {0, 0, 1};

glm::vec3 const INF3 = {INF, INF, INF};

} // namespace Vonsai
