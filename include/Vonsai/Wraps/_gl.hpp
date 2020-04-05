#pragma once

#include "../Utils/Logger.hpp"
#include <glad/glad.h>

// Verify OpenGL action
#define GL_ASSERT(funcToCheck) \
  glErrClear();                \
  funcToCheck;                 \
  glAssert(__FILE__, __LINE__);

// Avoid fake errors
static inline void glErrClear() {
  while (glGetError() != GL_NO_ERROR)
    ;
}

// Parse real errors after a gl func is executed
static inline void glAssert(char const *file, int line) {
  if (auto glError = glGetError()) {
    char const *errStr;
    switch (glError) {
      case GL_INVALID_ENUM: errStr = "Invalid enum"; break;
      case GL_INVALID_VALUE: errStr = "Invalid value"; break;
      case GL_INVALID_OPERATION: errStr = "Invalid operation"; break;
      default: errStr = "Unrecognised GLenum"; break;
    }
    vo_print("\nOPENGL ERROR - {}:{}\n↪ ({}) {}\n", file, line, errStr, glError);
  }
}
