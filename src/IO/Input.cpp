#include <Vonsai/IO/Input.hpp>

#include <Vonsai/IO/Window.hpp>
#include <Vonsai/Wraps/_glfw.hpp>

#include <Vonsai/Utils/Logger.hpp>

namespace Vonsai {

// - PUBLIC -

Input::Input() {
  m_keys.reserve(KeyCode::Last);
  m_keysPrev.reserve(KeyCode::Last);
}

bool Input::clickHoldL() const { return m_click.L; }
bool Input::clickHoldR() const { return m_click.R; }
bool Input::clickHoldM() const { return m_click.M; }

bool Input::clickPressL() const {
  bool state    = !m_click.prevL and m_click.L;
  m_click.prevL = m_click.L;
  return state;
}
bool Input::clickPressR() const {
  bool state    = !m_click.prevR and m_click.R;
  m_click.prevR = m_click.R;
  return state;
}
bool Input::clickPressM() const {
  bool state    = !m_click.prevM and m_click.M;
  m_click.prevM = m_click.M;
  return state;
}

int Input::axisV() const { return m_axis.V; }
int Input::axisH() const { return m_axis.H; }

int Input::scrollV() const { return (anyShiftHold()) ? m_scroll.H : m_scroll.V; }
int Input::scrollH() const { return (anyShiftHold()) ? m_scroll.V : m_scroll.H; }

void Input::resetScrollAndAxis() {
  m_scroll.V = 0.f;
  m_scroll.H = 0.f;
  m_axis.V   = 0.f;
  m_axis.H   = 0.f;
}

bool Input::keyPress(int a_keyCode) const {
  if (m_keys.count(a_keyCode) > 0) {
    bool state            = (m_keysPrev[a_keyCode] != 1) and (m_keys.at(a_keyCode) == 1); /*PRESSED*/
    m_keysPrev[a_keyCode] = m_keys.at(a_keyCode);
    return state;
  };
  return false;
}

bool Input::anyShiftPress() const { return keyPress(KeyCode::LeftShift) or keyPress(KeyCode::RightShift); }
bool Input::anyAltPress() const { return keyPress(KeyCode::LeftAlt) or keyPress(KeyCode::RightAlt); }
bool Input::anyCtrlPress() const { return keyPress(KeyCode::LeftCtrl) or keyPress(KeyCode::RightCtrl); }
bool Input::anySuperPress() const { return keyPress(KeyCode::LeftSuper) or keyPress(KeyCode::RightSuper); }

bool Input::keyHold(int a_keyCode) const {
  if (m_keys.count(a_keyCode) > 0) { return m_keys.at(a_keyCode) > 0; };
  return false;
}

bool Input::anyShiftHold() const { return keyHold(KeyCode::LeftShift) or keyHold(KeyCode::RightShift); }
bool Input::anyAltHold() const { return keyHold(KeyCode::LeftAlt) or keyHold(KeyCode::RightAlt); }
bool Input::anyCtrlHold() const { return keyHold(KeyCode::LeftCtrl) or keyHold(KeyCode::RightCtrl); }
bool Input::anySuperHold() const { return keyHold(KeyCode::LeftSuper) or keyHold(KeyCode::RightSuper); }

// - PRIVATE -

void onClickL(bool a_state, Input &a_ref) { a_ref.m_click.L = a_state; }
void onClickR(bool a_state, Input &a_ref) { a_ref.m_click.R = a_state; }
void onClickM(bool a_state, Input &a_ref) { a_ref.m_click.M = a_state; }
void onScroll(float a_displX, float a_displY, Input &a_ref) {
  a_ref.m_scroll.H = a_displX;
  a_ref.m_scroll.V = a_displY;
}
void onCursorMove(double a_x, double a_y, Input &a_ref) {
  if (a_ref.clickHoldM()) {
    a_ref.m_axis.H = a_x - a_ref.m_x;
    a_ref.m_axis.V = a_y - a_ref.m_y;
  }
  a_ref.m_x = a_x;
  a_ref.m_y = a_y;
}

void onKeyRelease(int a_key, Input &a_ref) { a_ref.m_keys[a_key] = 0u; }
void onKeyPress(int a_key, Input &a_ref) { a_ref.m_keys[a_key] = 1u; }
void onKeyHold(int a_key, Input &a_ref) { a_ref.m_keys[a_key] = 2u; }

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
    switch (action) {
      case GLFW_RELEASE: onKeyRelease(key, *curr); break;
      case GLFW_PRESS: onKeyPress(key, *curr); break;
      case GLFW_REPEAT: onKeyHold(key, *curr); break;
      default: break;
    }
  });
}

// ATTORNEY ( https://en.wikibooks.org/wiki/More_C++_Idioms/Friendship_and_the_Attorney-Client )
void InputAttorney::linkCallbacks(Input *a_input, void *a_window) {
  s_inputPtrs.push_back(a_input);
  a_input->linkCallbacks(a_window);
}

} // namespace Vonsai
