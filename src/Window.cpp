#include <Vonsai/Window.hpp>

#include <iostream>
#include <mutex>

// #include <Vonsai/Utils/Colors.hpp>
#include <Vonsai/Utils/Logger.hpp>


namespace Vonsai {

void Window::run() {
  while (s_list.size() > 0) {
    for (Window *window : s_list) { window->draw(); }

    // Update
    s_list.remove_if([](Window *const &w) {
      bool cond = w->shouldClose();
      if (cond) w->destroy();
      return cond;
    });
  }
}

void Window::updateFPS() {
  ++m_frames;
  double currTime = glfwGetTime();
  if (currTime - m_timeStamp >= 1.0) {
    glfwSetWindowTitle(m_ptr, (m_title + " :: " + std::to_string(m_frames) + "fps").c_str());
    m_timeStamp = currTime;
    m_frames    = 0;
  }
}

void Window::draw() {
  glfwPollEvents(); // glfwWaitEvents();
  if (!m_focused) return;
  activate();
  updateFPS();
  m_onRender();
  glfwSwapBuffers(m_ptr);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::setClearColor(glm::vec3 const &a_clearColor) {
  activate();
  m_clearColor = a_clearColor;
  glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.f);
}

Window::Window(int a_width, int a_height, std::string a_title, InputFn a_onInput) {
  static std::once_flag initGflw; /* clang-format off */
  std::call_once(initGflw, []() {
    if (!glfwInit()) { vo_err("Couldn't initialize GLFW"); return; }
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }); /* clang-format on */

  // Window creation
  auto ptr = glfwCreateWindow(a_width, a_height, a_title.c_str(), nullptr, nullptr);

  if (!ptr) {
    vo_err("Couldn't create window {}", a_title);
    glfwSetWindowShouldClose(ptr, GLFW_TRUE);
    glfwDestroyWindow(ptr);
    return;
  }
  glfwMakeContextCurrent(ptr);

  // Load extensions on current window
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    vo_err("Couldn't load OpenGL extensions");
    glfwSetWindowShouldClose(ptr, GLFW_TRUE);
    glfwDestroyWindow(ptr);
    return;
  }
  glFrontFace(GL_CCW);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // Set variables
  m_ptr         = ptr;
  m_valid       = true;
  m_title       = a_title;
  m_aspectRatio = static_cast<float>(a_width) / static_cast<float>(a_height);
  glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.f);

  // Set callbacks
  m_onInput = a_onInput;
  glfwSetWindowUserPointer(m_ptr, this);

  glfwSetKeyCallback(m_ptr, [](GLFWwindow *ptr, int key, int scancode, int action, int modifier) {
    auto curr = static_cast<Window *>(glfwGetWindowUserPointer(ptr));
    onKeyboardInput(modifier, *curr);
  });

  glfwSetWindowSizeCallback(m_ptr, [](GLFWwindow *ptr, int width, int height) {
    auto curr = static_cast<Window *>(glfwGetWindowUserPointer(ptr));
    onWindowResize(static_cast<float>(width), static_cast<float>(height), *curr);
  });

  glfwSetWindowFocusCallback(m_ptr, [](GLFWwindow *ptr, int focused) {
    auto curr = static_cast<Window *>(glfwGetWindowUserPointer(ptr));
    onWindowFocus(static_cast<bool>(focused), *curr);
  });

  glfwSetCursorPosCallback(m_ptr, [](GLFWwindow *ptr, double posX, double posY) {
    auto curr = static_cast<Window *>(glfwGetWindowUserPointer(ptr));
    if (curr->mouseClick.l) {
      float displX = (posY - curr->mousePos.y);
      float displY = (posX - curr->mousePos.x);
      curr->camera.pivot.rot += glm::vec3{displX, displY, 0.f};
      // curr->_onMove(displX, displY); // TODO
    }
    curr->mousePos.x = posX;
    curr->mousePos.y = posY;
  });

  glfwSetMouseButtonCallback(m_ptr, [](GLFWwindow *ptr, int button, int action, int modifier) {
    auto curr = static_cast<Window *>(glfwGetWindowUserPointer(ptr));
    bool val  = static_cast<bool>(action);
    switch (button) {
      case GLFW_MOUSE_BUTTON_LEFT: curr->mouseClick.l = val; break;
      case GLFW_MOUSE_BUTTON_MIDDLE: curr->mouseClick.m = val; break;
      case GLFW_MOUSE_BUTTON_RIGHT: curr->mouseClick.r = val; break;
      default: vo_warn("Only Left, Mid & Right buttons of the mouse are defined"); break;
    }
  });

  glfwSetScrollCallback(m_ptr, [](GLFWwindow *ptr, double displX, double displY) {
    auto curr = static_cast<Window *>(glfwGetWindowUserPointer(ptr));
    curr->camera.zoom(displY);
    // curr->_onScroll(displX, displY); // TODO
  });

  s_list.push_back(this);
}

Window::~Window() {
  if (!shouldClose()) {
    requestClose();
    destroy();
  }
}


/* FRIENDS */

void onKeyboardInput(int a_mod, Window &a_window) {
  if (a_window.getKey(GLFW_KEY_ESCAPE)) { a_window.requestClose(); } // Press ESC to close

  // Camera movement
  if (a_window.getKey(GLFW_KEY_E)) { a_window.camera.movement.U = true; }   // Up
  if (!a_window.getKey(GLFW_KEY_E)) { a_window.camera.movement.U = false; } // Up

  if (a_window.getKey(GLFW_KEY_Q)) { a_window.camera.movement.D = true; }   // Down
  if (!a_window.getKey(GLFW_KEY_Q)) { a_window.camera.movement.D = false; } // Down

  if (a_window.getKey(GLFW_KEY_W)) { a_window.camera.movement.F = true; }   // Front
  if (!a_window.getKey(GLFW_KEY_W)) { a_window.camera.movement.F = false; } // Front

  if (a_window.getKey(GLFW_KEY_S)) { a_window.camera.movement.B = true; }   // Back
  if (!a_window.getKey(GLFW_KEY_S)) { a_window.camera.movement.B = false; } // Back

  if (a_window.getKey(GLFW_KEY_D)) { a_window.camera.movement.R = true; }   // Right
  if (!a_window.getKey(GLFW_KEY_D)) { a_window.camera.movement.R = false; } // Right

  if (a_window.getKey(GLFW_KEY_A)) { a_window.camera.movement.L = true; }   // Left
  if (!a_window.getKey(GLFW_KEY_A)) { a_window.camera.movement.L = false; } // Left

  if (a_window.getKey(GLFW_KEY_0)) { a_window.camera.pivot.reset(); }

  // Random clear color
  // if (a_window.getKey(GLFW_KEY_R)) { a_window.setClearColor(Colors::random()); }

  a_window.m_onInput(a_mod, a_window);
}

void onWindowFocus(bool a_isFocused, Window &a_window) {
  a_window.m_focused = a_isFocused;
  if (a_window.m_focused) a_window.s_active = &a_window;
}

void onWindowResize(float a_width, float a_height, Window &a_window) { a_window.m_aspectRatio = a_width / a_height; }

std::ostream &operator<<(std::ostream &a_os, Window const &a_window) {
  std::string info = vo_fmt("{}", a_window.m_title);
  a_os << info;
  return a_os;
}

} // namespace Vonsai
