#pragma once

#include "../Utils/Logger.hpp"
#include <glad/glad.h>


namespace Vonsai::GL {

inline bool loadExtensions(void *a_proc) { return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(a_proc)); }

inline void defaultSetup() {
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  // glDepthFunc(GL_ALWAYS);

  // glEnable(GL_POLYGON_SMOOTH);

  // glEnable(GL_SCISSOR_TEST);
  // glScissor(500, 500, 500, 500);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // glClearColor(.2f, .2f, .2f, 1.f);
}

inline void clearColor(float r, float g, float b) { glClearColor(r, g, b, 1.f); }

} // namespace Vonsai::GL


// Verify OpenGL action
// TODO : Check if exists a context before run the func...
#define GL_ASSERT(funcToCheck)                                                            \
  while (glGetError() != GL_NO_ERROR)                                                     \
    ;                                                                                     \
  funcToCheck;                                                                            \
  if (auto glError = glGetError()) {                                                      \
    char const *errStr;                                                                   \
    switch (glError) {                                                                    \
      case GL_INVALID_ENUM: errStr = "Invalid enum"; break;                               \
      case GL_INVALID_VALUE: errStr = "Invalid value"; break;                             \
      case GL_INVALID_OPERATION: errStr = "Invalid operation"; break;                     \
      default: errStr = "Unrecognised GLenum"; break;                                     \
    }                                                                                     \
    vo_print("\nOPENGL ERROR - {}:{}\n↪ ({}) {}\n", __FILE__, __LINE__, errStr, glError); \
  }

#define GL_CALL(fn, ...)                                                            \
  do {                                                                              \
    while (glGetError() != GL_NO_ERROR)                                             \
      ;                                                                             \
    fn(__VA_ARGS__);                                                                \
    if (auto errCode = glGetError()) {                                              \
      char const *errMsg;                                                           \
      switch (errCode) {                                                            \
        case GL_INVALID_ENUM: errMsg = "Invalid enum"; break;                       \
        case GL_INVALID_VALUE: errMsg = "Invalid value"; break;                     \
        case GL_INVALID_OPERATION: errMsg = "Invalid operation"; break;             \
        default: errMsg = "Unrecognised GLenum"; break;                             \
      }                                                                             \
      vo_print("\nOPENGL ERROR - {}:{}\n↪ ({}) {}\n", file, line, errCode, errMsg); \
    }                                                                               \
  } while (void(0), 0)
