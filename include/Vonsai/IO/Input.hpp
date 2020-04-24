#pragma once

#include <unordered_map>
#include <vector>

namespace Vonsai {
class Window; // ! Forward decl

namespace KeyCode {
  // The unknown key
  constexpr int Unknown{-1};

  // Printable keys
  constexpr int Space{32};
  constexpr int Apostrophe{39}; // " ' "
  constexpr int Comma{44};      // " , "
  constexpr int Minus{45};      // " - "
  constexpr int Period{46};     // " . "
  constexpr int Slash{47};      // " / "
  constexpr int Num0{48};
  constexpr int Num1{49};
  constexpr int Num2{50};
  constexpr int Num3{51};
  constexpr int Num4{52};
  constexpr int Num5{53};
  constexpr int Num6{54};
  constexpr int Num7{55};
  constexpr int Num8{56};
  constexpr int Num9{57};
  constexpr int Semicolon{59}; // " ; "
  constexpr int Equal{61};     // " = "
  constexpr int A{65};
  constexpr int B{66};
  constexpr int C{67};
  constexpr int D{68};
  constexpr int E{69};
  constexpr int F{70};
  constexpr int G{71};
  constexpr int H{72};
  constexpr int I{73};
  constexpr int J{74};
  constexpr int K{75};
  constexpr int L{76};
  constexpr int M{77};
  constexpr int N{78};
  constexpr int O{79};
  constexpr int P{80};
  constexpr int Q{81};
  constexpr int R{82};
  constexpr int S{83};
  constexpr int T{84};
  constexpr int U{85};
  constexpr int V{86};
  constexpr int W{87};
  constexpr int X{88};
  constexpr int Y{89};
  constexpr int Z{90};
  constexpr int LeftBracket{91};  // " [ "
  constexpr int Backslash{92};    // " \ "
  constexpr int RightBracket{93}; // " ] "
  constexpr int GraveAccent{96};  // " ` "

  /* Function keys */
  constexpr int Esc{256};
  constexpr int Enter{257};
  constexpr int Tab{258};
  constexpr int Backspace{259};
  constexpr int Insert{260};
  constexpr int Delete{261};
  constexpr int Right{262};
  constexpr int Left{263};
  constexpr int Down{264};
  constexpr int Up{265};
  constexpr int PageUp{266};
  constexpr int PageDown{267};
  constexpr int Home{268};
  constexpr int End{269};
  constexpr int CapsLock{280};
  constexpr int ScrollLock{281};
  constexpr int NumLock{282};
  constexpr int PrintScreen{283};
  constexpr int Pause{284};
  constexpr int F1{290};
  constexpr int F2{291};
  constexpr int F3{292};
  constexpr int F4{293};
  constexpr int F5{294};
  constexpr int F6{295};
  constexpr int F7{296};
  constexpr int F8{297};
  constexpr int F9{298};
  constexpr int F10{299};
  constexpr int F11{300};
  constexpr int F12{301};
  constexpr int F13{302};
  constexpr int F14{303};
  constexpr int F15{304};
  constexpr int F16{305};
  constexpr int F17{306};
  constexpr int F18{307};
  constexpr int F19{308};
  constexpr int F20{309};
  constexpr int F21{310};
  constexpr int F22{311};
  constexpr int F23{312};
  constexpr int F24{313};
  constexpr int F25{314};
  constexpr int Kp0{320};
  constexpr int Kp1{321};
  constexpr int Kp2{322};
  constexpr int Kp3{323};
  constexpr int Kp4{324};
  constexpr int Kp5{325};
  constexpr int Kp6{326};
  constexpr int Kp7{327};
  constexpr int Kp8{328};
  constexpr int Kp9{329};
  constexpr int KpDecimal{330};
  constexpr int KpDivide{331};
  constexpr int KpMultiply{332};
  constexpr int KpSubtract{333};
  constexpr int KpAdd{334};
  constexpr int KpEnter{335};
  constexpr int KpEqual{336};
  constexpr int LeftShift{340};
  constexpr int LeftCtrl{341};
  constexpr int LeftAlt{342};
  constexpr int LeftSuper{343};
  constexpr int RightShift{344};
  constexpr int RightCtrl{345};
  constexpr int RightAlt{346};
  constexpr int RightSuper{347};
  constexpr int Menu{348};

  constexpr int Last{Menu};
}; // namespace KeyCode

class Input {
public:
  explicit Input() = default;

  bool clickL() const;
  bool clickR() const;
  bool clickM() const;

  int  axisV() const;
  int  axisH() const;
  int  scrollV() const;
  int  scrollH() const;
  void resetScrollAndAxis();

  bool anyShift() const;
  bool anyAlt() const;
  bool anyCtrl() const;
  bool anySuper() const;

  bool key(int a_keyCode) const;

private:
  static inline Input *s_inputPtr{nullptr}; // TODO : To support many move this to a vector

  struct {
    float H{0.f}; // -1 / 0 / 1
    float V{0.f}; // -1 / 0 / 1
  } m_axis;
  double m_x{0.0};
  double m_y{0.0};

  struct {
    float H{0.f}; // -1 / 0 / 1
    float V{0.f}; // -1 / 0 / 1
  } m_scroll;

  struct {
    bool L{false};
    bool R{false};
    bool M{false};
  } m_click;

  friend void onClickL(bool a_state, Input &a_ref);
  friend void onClickR(bool a_state, Input &a_ref);
  friend void onClickM(bool a_state, Input &a_ref);
  friend void onScroll(float a_displX, float a_displY, Input &a_ref);
  friend void onCursorMove(double a_x, double a_y, Input &a_ref);

  std::unordered_map<int, bool> m_keys;
  friend void                   onKeyPress(int a_key, Input &a_ref);
  friend void                   onKeyRelease(int a_key, Input &a_ref);

  void linkCallbacks(void *a_window);

  // Friends of this class:
  friend class InputAttorney;
};

class InputAttorney {
private:
  friend class Input;
  static inline std::vector<Input *> s_inputPtrs{};

  friend class Window;
  static void linkCallbacks(Input *a_input, void *a_window);
};

} // namespace Vonsai
