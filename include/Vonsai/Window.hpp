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
private:
  static inline std::list<Window *> s_list{};
  static inline Window *            s_active{nullptr};

  using RenderFn = std::function<void(void)>;
  static inline RenderFn s_nullRenderFn{[]() {}};

  using InputFn = std::function<void(int, Window &)>;
  static inline InputFn s_nullInputFn{[](int, Window &) {}};

  GLFWwindow *m_ptr{nullptr};
  InputFn     m_onInput{s_nullInputFn};
  RenderFn    m_onRender{s_nullRenderFn};

  bool        m_valid{false};
  bool        m_focused{false};
  float       m_aspectRatio{1.3f};
  std::string m_title{"unnamed window"};
  glm::vec3   m_clearColor{.8f, .5f, .8f};

  unsigned int m_frames{0u};
  double       m_timeStamp{glfwGetTime()};

  void        updateFPS();
  inline void destroy() { glfwDestroyWindow(m_ptr); }
  inline void activate() { glfwMakeContextCurrent(m_ptr); }

  friend void          onKeyboardInput(int a_mod, Window &a_window);
  friend void          onWindowFocus(bool a_isFocused, Window &a_window);
  friend void          onWindowResize(float a_width, float a_height, Window &a_window);
  friend std::ostream &operator<<(std::ostream &a_os, Window const &a_window);

public:
  Camera camera;

  /* clang-format off */
  struct { double x{0.0}, y{0.0}; } mousePos;
  struct { bool l{false}, m{false}, r{false}; } mouseClick;
  /* clang-format on */

  // INFO
  inline bool  isValid() const { return m_valid; }
  inline bool  isFocused() const { return m_focused; }
  inline float getAspectRatio() const { return m_aspectRatio; }
  inline bool  shouldClose() { return glfwWindowShouldClose(m_ptr); }
  inline bool  getKey(int a_key) const { return bool(glfwGetKey(m_ptr, a_key)); }

  // ACTIONS
  static void                  run();
  void                         draw();
  static inline Window *const &active() { return s_active; }
  void                         setClearColor(glm::vec3 const &a_ClearColor);
  inline void                  requestClose() { glfwSetWindowShouldClose(m_ptr, GLFW_TRUE); }
  inline void                  setInputFn(InputFn a_func) { m_onInput = (!a_func) ? s_nullInputFn : a_func; }
  inline void                  setRenderFn(RenderFn a_func) { m_onRender = (!a_func) ? s_nullRenderFn : a_func; }

  // DES/CONSTRUCTOR + ASSIGN OPERATORS
  Window(int a_width, int a_height, std::string a_title, InputFn a_onInput = s_nullInputFn);
  ~Window();
  Window(Window &&) = delete;
  Window &operator=(Window &&) = delete;
  Window(Window const &)       = delete;
  Window &operator=(Window const &) = delete;
};

} // namespace Vonsai
