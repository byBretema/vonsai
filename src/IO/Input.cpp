#include <Vonsai/IO/Input.hpp>

#include <Vonsai/IO/Window.hpp>
#include <Vonsai/Wraps/_glfw.hpp>

#include <iostream>

namespace Vonsai {
Input::Input() { s_inputPtr = this; }

bool Input::clickL() const { return m_click.L; }
bool Input::clickR() const { return m_click.R; }
bool Input::clickM() const { return m_click.M; }

int Input::axisV() const { return m_axis.V; }
int Input::axisH() const { return m_axis.H; }

int Input::scrollV() const { return (anyShift()) ? m_scroll.H : m_scroll.V; }
int Input::scrollH() const { return (anyShift()) ? m_scroll.V : m_scroll.H; }

void Input::resetScrollAndAxis() {
  m_scroll.V = 0.f;
  m_scroll.H = 0.f;
  m_axis.V   = 0.f;
  m_axis.H   = 0.f;
}

void onClickL(bool a_state, Input &a_ref) { a_ref.m_click.L = a_state; }
void onClickR(bool a_state, Input &a_ref) { a_ref.m_click.R = a_state; }
void onClickM(bool a_state, Input &a_ref) { a_ref.m_click.M = a_state; }
void onScroll(float a_displX, float a_displY, Input &a_ref) {
  a_ref.m_scroll.H = a_displX;
  a_ref.m_scroll.V = a_displY;
}
void onCursorMove(double a_x, double a_y, Input &a_ref) {
  if (a_ref.clickL()) {
    a_ref.m_axis.H = a_x - a_ref.m_x;
    a_ref.m_axis.V = a_y - a_ref.m_y;
  }
  a_ref.m_x = a_x;
  a_ref.m_y = a_y;
}

bool Input::key(int a_keyCode) const {
  if (m_keys.count(a_keyCode) > 0) { return m_keys.at(a_keyCode); };
  return false;
}

bool Input::anyShift() const { return key(KeyCode::LeftShift) or key(KeyCode::RightShift); }
bool Input::anyAlt() const { return key(KeyCode::LeftAlt) or key(KeyCode::RightAlt); }
bool Input::anyCtrl() const { return key(KeyCode::LeftCtrl) or key(KeyCode::RightCtrl); }
bool Input::anySuper() const { return key(KeyCode::LeftSuper) or key(KeyCode::RightSuper); }

void onKeyPress(int a_key, Input &a_ref) { a_ref.m_keys[a_key] = true; }
void onKeyRelease(int a_key, Input &a_ref) { a_ref.m_keys[a_key] = false; }

void Input::linkCallbacks(void *a_window) {
  auto GLFW_PTR = static_cast<GLFWwindow *>(a_window);

  // Mouse

  glfwSetCursorPosCallback(GLFW_PTR, []([[maybe_unused]] GLFWwindow *ptr, double posX, double posY) {
    auto curr = InputAttorney::s_inputPtrs[InputAttorney::s_inputPtrs.size() - 1];
    onCursorMove(posX, posY, *curr);
  });

  glfwSetMouseButtonCallback(
      GLFW_PTR, []([[maybe_unused]] GLFWwindow *ptr, int button, int action, [[maybe_unused]] int modifier) {
        auto curr  = InputAttorney::s_inputPtrs[InputAttorney::s_inputPtrs.size() - 1];
        bool state = static_cast<bool>(action);
        switch (button) {
          case GLFW_MOUSE_BUTTON_LEFT: onClickL(state, *curr); break;
          case GLFW_MOUSE_BUTTON_RIGHT: onClickR(state, *curr); break;
          case GLFW_MOUSE_BUTTON_MIDDLE: onClickM(state, *curr); break;
          default: vo_warn("Only Left, Mid & Right buttons of the mouse are defined"); break;
        }
      });

  glfwSetScrollCallback(GLFW_PTR, []([[maybe_unused]] GLFWwindow *ptr, double displX, double displY) {
    auto curr = InputAttorney::s_inputPtrs[InputAttorney::s_inputPtrs.size() - 1];
    onScroll(displX, displY, *curr);
  });

  // Keyboard

  glfwSetKeyCallback(GLFW_PTR, []([[maybe_unused]] GLFWwindow *ptr, int key, [[maybe_unused]] int scancode, int action,
                                  [[maybe_unused]] int modifier) {
    auto curr = InputAttorney::s_inputPtrs[InputAttorney::s_inputPtrs.size() - 1];
    (action) ? onKeyPress(key, *curr) : onKeyRelease(key, *curr);
  });
}

// ATTORNEY ( https://en.wikibooks.org/wiki/More_C++_Idioms/Friendship_and_the_Attorney-Client )
void InputAttorney::linkCallbacks(Input *a_input, void *a_window) {
  s_inputPtrs.push_back(a_input);
  a_input->linkCallbacks(a_window);
}

} // namespace Vonsai
