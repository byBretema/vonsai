#pragma once

#include "Camera.hpp"
#include "Wraps/_glm.hpp"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <list>
#include <string>

namespace Vonsai {

class Window {
  friend class App;

private:
  // Manager ....
  static inline std::list<Window *> s_list{};
  static inline Window *            s_active{nullptr};


  // Graphic context ....
  using RenderFn = std::function<void(void)>; // ! Is render window responsibility?
  static inline RenderFn s_nullRenderFn{[]() {}};
  RenderFn               m_onRender{s_nullRenderFn};

  glm::vec3 m_clearColor{1.f, 0.f, 1.f};

  unsigned int m_frames{0u};
  double       m_timeStamp{glfwGetTime()};
  double       m_deltaTime{glfwGetTime()};

  void updateFPS();


  // Input ....
  using InputFn = std::function<void(int, Window &)>;
  static inline InputFn s_nullInputFn{[](int, Window &) {}};
  InputFn               m_onInput{s_nullInputFn};
  friend void           onKeyboardInput(int a_mod, Window &a_window);


  // Window ....
  GLFWwindow *m_ptr{nullptr};

  bool m_valid{false}; // ? Group on STATE with enum values?
  bool m_focused{false};

  float       m_aspectRatio{1.3f};
  std::string m_title{"unnamed window"};

  inline void destroy() { glfwDestroyWindow(m_ptr); }
  inline void activate() { glfwMakeContextCurrent(m_ptr); }
  inline bool shouldClose() { return glfwWindowShouldClose(m_ptr); }

  friend void onWindowFocus(bool a_isFocused, Window &a_window);
  friend void onWindowResize(float a_width, float a_height, Window &a_window);

  Window(int a_width, int a_height, std::string a_title, InputFn a_onInput = s_nullInputFn);

  friend std::ostream &operator<<(std::ostream &a_os, Window const &a_window);


public:
  Camera camera; // ! Who should have the camera, or how to access if it is not member ?

  // Manager...
  static void                  run();
  static inline Window *const &active() { return s_active; }


  // Input ...
  /* clang-format off */
  struct { double x{0.0}, y{0.0}; } mousePos;
  struct { bool l{false}, m{false}, r{false}; } mouseClick;
  /* clang-format on */

  inline bool getKey(int a_key) const { return bool(glfwGetKey(m_ptr, a_key)); }

  inline void setInputFn(InputFn a_func) { m_onInput = (!a_func) ? s_nullInputFn : a_func; } // ! Really needed?


  // Window ....
  inline float getAspectRatio() const { return m_aspectRatio; }
  // TODO: getState() that return state... 0:invalid, 1:inactive, 2:focused
  inline void requestClose() { glfwSetWindowShouldClose(m_ptr, GLFW_TRUE); }


  // Graphic context ....
  void        draw();
  void        setClearColor(glm::vec3 const &a_ClearColor);
  inline void setRenderFn(RenderFn a_func) { m_onRender = (!a_func) ? s_nullRenderFn : a_func; }


  // DESTRUCTOR + ASSIGN OPERATORS
  ~Window();
  Window(Window &&) = delete;
  Window &operator=(Window &&) = delete;
  Window(Window const &)       = delete;
  Window &operator=(Window const &) = delete;
};

} // namespace Vonsai
