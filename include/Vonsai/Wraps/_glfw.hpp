#pragma once

#include "_gl.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../Utils/Logger.hpp"

static inline const int initGLFW = []() {
  if (!glfwInit()) {
    vo_err("Couldn't initialize GLFW");
    return 0;
  }
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4 /* a_versionMajor */);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1 /* a_versionMinor */);
  glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  return 1;
}();
