#pragma once

#include <array>
#include <list>
#include <string>
#include <unordered_map>

#include "Scene.hpp"

namespace Vonsai {

struct KeyCodeT {
  // The unknown key
  int const Unknown{-1};

  // Printable keys
  int const Space{32};
  int const Apostrophe{39}; // " ' "
  int const Comma{44};      // " , "
  int const Minus{45};      // " - "
  int const Period{46};     // " . "
  int const Slash{47};      // " / "
  int const Num0{48};
  int const Num1{49};
  int const Num2{50};
  int const Num3{51};
  int const Num4{52};
  int const Num5{53};
  int const Num6{54};
  int const Num7{55};
  int const Num8{56};
  int const Num9{57};
  int const Semicolon{59}; // " ; "
  int const Equal{61};     // " = "
  int const A{65};
  int const B{66};
  int const C{67};
  int const D{68};
  int const E{69};
  int const F{70};
  int const G{71};
  int const H{72};
  int const I{73};
  int const J{74};
  int const K{75};
  int const L{76};
  int const M{77};
  int const N{78};
  int const O{79};
  int const P{80};
  int const Q{81};
  int const R{82};
  int const S{83};
  int const T{84};
  int const U{85};
  int const V{86};
  int const W{87};
  int const X{88};
  int const Y{89};
  int const Z{90};
  int const LeftBracket{91};  // " [ "
  int const Backslash{92};    // " \ "
  int const RightBracket{93}; // " ] "
  int const GraveAccent{96};  // " ` "

  /* Function keys */
  int const Esc{256};
  int const Enter{257};
  int const Tab{258};
  int const Backspace{259};
  int const Insert{260};
  int const Delete{261};
  int const Right{262};
  int const Left{263};
  int const Down{264};
  int const Up{265};
  int const PageUp{266};
  int const PageDown{267};
  int const Home{268};
  int const End{269};
  int const CapsLock{280};
  int const ScrollLock{281};
  int const NumLock{282};
  int const PrintScreen{283};
  int const Pause{284};
  int const F1{290};
  int const F2{291};
  int const F3{292};
  int const F4{293};
  int const F5{294};
  int const F6{295};
  int const F7{296};
  int const F8{297};
  int const F9{298};
  int const F10{299};
  int const F11{300};
  int const F12{301};
  int const F13{302};
  int const F14{303};
  int const F15{304};
  int const F16{305};
  int const F17{306};
  int const F18{307};
  int const F19{308};
  int const F20{309};
  int const F21{310};
  int const F22{311};
  int const F23{312};
  int const F24{313};
  int const F25{314};
  int const Kp0{320};
  int const Kp1{321};
  int const Kp2{322};
  int const Kp3{323};
  int const Kp4{324};
  int const Kp5{325};
  int const Kp6{326};
  int const Kp7{327};
  int const Kp8{328};
  int const Kp9{329};
  int const KpDecimal{330};
  int const KpDivide{331};
  int const KpMultiply{332};
  int const KpSubtract{333};
  int const KpAdd{334};
  int const KpEnter{335};
  int const KpEqual{336};
  int const LeftShift{340};
  int const LeftCtrl{341};
  int const LeftAlt{342};
  int const LeftSuper{343};
  int const RightShift{344};
  int const RightCtrl{345};
  int const RightAlt{346};
  int const RightSuper{347};
  int const Menu{348};

  int const Last{Menu};
} constexpr KeyCode;

class IO {
  // * WINDOW
public:
  bool update();
  void close();

  bool  isValid() const;
  bool  isFocused() const;
  float aspectRatio() const;

private:
  static inline unsigned int   windowCounter = 0;
  static constexpr float       WIDTH         = 800.f;
  static constexpr float       HEIGHT        = 600.f;
  static constexpr const char *TITLE         = "VONSAI";

  void *               m_window{nullptr};
  std::string          m_title{TITLE};
  float                m_width{WIDTH};
  float                m_height{HEIGHT};
  std::array<float, 3> m_color{0.75f, 0.25f, 0.75f};

  bool m_valid{false};
  bool m_focused{false};

  void destroy();
  void activate();
  bool shouldClose();

  friend void onDestroy(IO &a_io);
  friend void onWindowFocus(bool a_isFocused, IO &a_io);
  friend void onWindowResize(float a_width, float a_height, IO &a_io);


  // * KEYBOARD
public:
  bool key(int a_keyCode) const;
  bool anyShift() const;
  bool anyAlt() const;
  bool anyCtrl() const;
  bool anySuper() const;

private:
  int                           m_mod;
  std::unordered_map<int, bool> m_keys;
  friend void                   onKeyPress(int a_key, IO &a_io);
  friend void                   onKeyRelease(int a_key, IO &a_io);


  // * MOUSE
public:
  bool clickL() const;
  bool clickR() const;
  bool clickM() const;

  int axisV() const;
  int axisH() const;

  int scrollV() const;
  int scrollH() const;

private:
  struct {
    float H{0.f}; // -1 / 0 / 1
    float V{0.f}; // -1 / 0 / 1
  } m_axis;
  double m_cursorX{0.0};
  double m_cursorY{0.0};

  struct {
    float H{0.f}; // -1 / 0 / 1
    float V{0.f}; // -1 / 0 / 1
  } m_scroll;

  struct {
    bool L{false};
    bool R{false};
    bool M{false};
  } m_click;

  friend void onClickL(bool a_state, IO &a_io);
  friend void onClickR(bool a_state, IO &a_io);
  friend void onClickM(bool a_state, IO &a_io);
  friend void onScroll(float a_displX, float a_displY, IO &a_io);
  friend void onCursorMove(double a_x, double a_y, IO &a_io);

  // * IO
public:
  explicit IO();

  std::shared_ptr<Scene> m_activeScene; // TODO: PRIVATE
private:
  std::list<std::shared_ptr<Scene>> m_scenes;
};

} // namespace Vonsai
