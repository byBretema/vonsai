#include <Vonsai/IO.hpp>

#include <iostream>
#include <mutex>

#include <Vonsai/Wraps/_gl.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Vonsai/Utils/Colors.hpp>
#include <Vonsai/Utils/Logger.hpp>


namespace Vonsai {

#define GLFW_PTR static_cast<GLFWwindow *>(m_window)

// * WINDOW

bool IO::update() {
  if (!m_focused) { glfwWaitEvents(); }
  activate();
  glfwPollEvents();

  m_activeScene->update();

  glfwSwapBuffers(GLFW_PTR);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return m_valid;
}

void IO::close() { destroy(); }
bool IO::isValid() const { return m_valid; }
bool IO::isFocused() const { return m_focused; }

void IO::destroy() {
  m_valid = false;
  glfwDestroyWindow(GLFW_PTR);
}
void IO::activate() { glfwMakeContextCurrent(GLFW_PTR); }
bool IO::shouldClose() { return glfwWindowShouldClose(GLFW_PTR); }

void onDestroy(IO &a_io) { a_io.destroy(); }
void onWindowFocus(bool a_focused, IO &a_io) { a_io.m_focused = a_focused; }
void onWindowResize(float a_width, float a_height, IO &a_io) {
  a_io.m_width  = a_width;
  a_io.m_height = a_height;
}


// * KEYBOARD
bool IO::key(int a_keyCode) const {
  if (m_keys.count(a_keyCode) > 0) { return m_keys.at(a_keyCode); };
  return false;
}
void onKeyPress(int a_key, IO &a_io) { a_io.m_keys[a_key] = true; }
void onKeyRelease(int a_key, IO &a_io) { a_io.m_keys[a_key] = false; }


// * MOUSE

bool IO::clickL() const { return m_click.L; }
bool IO::clickR() const { return m_click.R; }
bool IO::clickM() const { return m_click.M; }

int IO::axisV() const { return m_axis.V; }
int IO::axisH() const { return m_axis.H; }

int IO::scrollV() const { return m_scroll.V; }
int IO::scrollH() const { return m_scroll.H; }

void onClickL(bool a_state, IO &a_io) { a_io.m_click.L = a_state; }
void onClickR(bool a_state, IO &a_io) { a_io.m_click.R = a_state; }
void onClickM(bool a_state, IO &a_io) { a_io.m_click.M = a_state; }
void onScroll(float a_displX, float a_displY, IO &a_io) {
  a_io.m_scroll.V = (a_displY < 0.f) ? -1 : (a_displY > 0.f) ? 1 : 0;
  a_io.m_scroll.H = (a_displX < 0.f) ? -1 : (a_displX > 0.f) ? 1 : 0;
}
void onCursorMove(double a_x, double a_y, IO &a_io) {
  if (!a_io.clickL()) {
    auto displX   = a_x - a_io.m_cursorX;
    auto displY   = a_y - a_io.m_cursorY;
    a_io.m_axis.V = (displY < 0.f) ? -1 : (displY > 0.f) ? 1 : 0;
    a_io.m_axis.H = (displX < 0.f) ? -1 : (displX > 0.f) ? 1 : 0;
    // vo_print("cursor pos: {} / {}", a_io.m_cursorPrevX, a_io.m_cursorPrevY);
  }
  a_io.m_cursorX = a_x;
  a_io.m_cursorY = a_y;
}

// * IO
IO::IO() {
  static std::once_flag glfwInitFlag;
  std::call_once(glfwInitFlag, []() {
    if (!glfwInit()) {
      vo_err("Couldn't initialize GLFW");
      return;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4 /* a_versionMajor */);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1 /* a_versionMinor */);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  });


  // 1. WINDOW CREATION
  m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
  if (!m_window) {
    vo_err("Couldn't create window {}", m_title);
    return;
  }
  m_title += (std::string{" :: Window "} + std::to_string(++windowCounter));
  glfwSetWindowTitle(GLFW_PTR, m_title.c_str());


  // 2. LOAD EXTENSIONS ON THAT WINDOW
  activate();
  if (!GL::loadExtensions(reinterpret_cast<void *>(glfwGetProcAddress))) {
    vo_err("Couldn't load OpenGL extensions");
    glfwDestroyWindow(GLFW_PTR);
    m_window = nullptr;
    return;
  }

  // 3. LOAD DEFAULT OPENGL SETUP
  GL::defaultSetup();

  // 4. EVERYTHING GO AS EXPECTED
  m_valid = true;

  // 5. LINK OUR-WINDOW EVENTS TO GLFW-WINDOW
  glfwSetWindowUserPointer(GLFW_PTR, this); // ! MUST
  glfwSetKeyCallback(GLFW_PTR, [](GLFWwindow *ptr, int key, int scancode, int action, int modifier) {
    auto curr = static_cast<IO *>(glfwGetWindowUserPointer(ptr));
    (action) ? onKeyPress(key, *curr) : onKeyRelease(key, *curr);
  });
  glfwSetWindowSizeCallback(GLFW_PTR, [](GLFWwindow *ptr, int width, int height) {
    auto curr = static_cast<IO *>(glfwGetWindowUserPointer(ptr));
    onWindowResize(static_cast<float>(width), static_cast<float>(height), *curr);
  });
  glfwSetWindowFocusCallback(GLFW_PTR, [](GLFWwindow *ptr, int focused) {
    auto curr = static_cast<IO *>(glfwGetWindowUserPointer(ptr));
    onWindowFocus(static_cast<bool>(focused), *curr);
  });
  glfwSetCursorPosCallback(GLFW_PTR, [](GLFWwindow *ptr, double posX, double posY) {
    auto curr = static_cast<IO *>(glfwGetWindowUserPointer(ptr));
    onCursorMove(posX, posY, *curr);
  });
  glfwSetMouseButtonCallback(GLFW_PTR, [](GLFWwindow *ptr, int button, int action, int modifier) {
    auto curr  = static_cast<IO *>(glfwGetWindowUserPointer(ptr));
    bool state = static_cast<bool>(action);
    switch (button) {
      case GLFW_MOUSE_BUTTON_LEFT: onClickL(state, *curr); break;
      case GLFW_MOUSE_BUTTON_RIGHT: onClickR(state, *curr); break;
      case GLFW_MOUSE_BUTTON_MIDDLE: onClickM(state, *curr); break;
      default: vo_warn("Only Left, Mid & Right buttons of the mouse are defined"); break;
    }
  });
  glfwSetScrollCallback(GLFW_PTR, [](GLFWwindow *ptr, double displX, double displY) {
    auto curr = static_cast<IO *>(glfwGetWindowUserPointer(ptr));
    onScroll(displX, displY, *curr);
  });
  glfwSetWindowCloseCallback(GLFW_PTR, [](GLFWwindow *ptr) {
    auto curr = static_cast<IO *>(glfwGetWindowUserPointer(ptr));
    onDestroy(*curr);
  });

  // 6. CREATE A DEFAULT SCENE
  m_activeScene = m_scenes.emplace_back(std::make_shared<Scene>());
}

} // namespace Vonsai



// ! Esto es cosa del engine :D
// void Window::updateFPS() {
//   ++m_frames;
//   double currTime = glfwGetTime();
//   if (currTime - m_timeStamp >= 1.0) {
//     glfwSetWindowTitle(m_ptr, (m_title + " :: " + std::to_string(m_frames) + "fps").c_str());
//     m_timeStamp = currTime;
//     m_frames    = 0;
//   }
// }



// void Window::setClearColor(glm::vec3 const &a_clearColor) {
//   activate();
//   m_clearColor = a_clearColor;
//   glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.f);
// }


/* FRIENDS */

// void onKeyboardInput(int a_mod, Window &a_window) {
//   if (a_window.getKey(GLFW_KEY_ESCAPE)) { a_window.requestClose(); } // Press ESC to close

//   // Camera movement
//   if (a_window.getKey(GLFW_KEY_E)) { a_window.camera.movement.U = true; }   // Up
//   if (!a_window.getKey(GLFW_KEY_E)) { a_window.camera.movement.U = false; } // Up

//   if (a_window.getKey(GLFW_KEY_Q)) { a_window.camera.movement.D = true; }   // Down
//   if (!a_window.getKey(GLFW_KEY_Q)) { a_window.camera.movement.D = false; } // Down

//   if (a_window.getKey(GLFW_KEY_W)) { a_window.camera.movement.F = true; }   // Front
//   if (!a_window.getKey(GLFW_KEY_W)) { a_window.camera.movement.F = false; } // Front

//   if (a_window.getKey(GLFW_KEY_S)) { a_window.camera.movement.B = true; }   // Back
//   if (!a_window.getKey(GLFW_KEY_S)) { a_window.camera.movement.B = false; } // Back

//   if (a_window.getKey(GLFW_KEY_D)) { a_window.camera.movement.R = true; }   // Right
//   if (!a_window.getKey(GLFW_KEY_D)) { a_window.camera.movement.R = false; } // Right

//   if (a_window.getKey(GLFW_KEY_A)) { a_window.camera.movement.L = true; }   // Left
//   if (!a_window.getKey(GLFW_KEY_A)) { a_window.camera.movement.L = false; } // Left

//   if (a_window.getKey(GLFW_KEY_0)) { a_window.camera.pivot.reset(); }

//   // Random clear color
//   // if (a_window.getKey(GLFW_KEY_R)) { a_window.setClearColor(Colors::random()); }

//   a_window.m_onInput(a_mod, a_window);
// }
