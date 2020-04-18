#pragma once

#include <array>
#include <list>
#include <string>
#include <unordered_map>

#include "Scene.hpp"

namespace Vonsai {

struct KeyCode {
  // The unknown key
  static int const Unknown{-1};

  // Printable keys
  static int const Space{32};
  static int const Apostrophe{39}; // " ' "
  static int const Comma{44};      // " , "
  static int const Minus{45};      // " - "
  static int const Period{46};     // " . "
  static int const Slash{47};      // " / "
  static int const Num0{48};
  static int const Num1{49};
  static int const Num2{50};
  static int const Num3{51};
  static int const Num4{52};
  static int const Num5{53};
  static int const Num6{54};
  static int const Num7{55};
  static int const Num8{56};
  static int const Num9{57};
  static int const Semicolon{59}; // " ; "
  static int const Equal{61};     // " = "
  static int const A{65};
  static int const B{66};
  static int const C{67};
  static int const D{68};
  static int const E{69};
  static int const F{70};
  static int const G{71};
  static int const H{72};
  static int const I{73};
  static int const J{74};
  static int const K{75};
  static int const L{76};
  static int const M{77};
  static int const N{78};
  static int const O{79};
  static int const P{80};
  static int const Q{81};
  static int const R{82};
  static int const S{83};
  static int const T{84};
  static int const U{85};
  static int const V{86};
  static int const W{87};
  static int const X{88};
  static int const Y{89};
  static int const Z{90};
  static int const LeftBracket{91};  // " [ "
  static int const Backslash{92};    // " \ "
  static int const RightBracket{93}; // " ] "
  static int const GraveAccent{96};  // " ` "

  /* Function keys */
  static int const Esc{256};
  static int const Enter{257};
  static int const Tab{258};
  static int const Backspace{259};
  static int const Insert{260};
  static int const Delete{261};
  static int const Right{262};
  static int const Left{263};
  static int const Down{264};
  static int const Up{265};
  static int const PageUp{266};
  static int const PageDown{267};
  static int const Home{268};
  static int const End{269};
  static int const CapsLock{280};
  static int const ScrollLock{281};
  static int const NumLock{282};
  static int const PrintScreen{283};
  static int const Pause{284};
  static int const F1{290};
  static int const F2{291};
  static int const F3{292};
  static int const F4{293};
  static int const F5{294};
  static int const F6{295};
  static int const F7{296};
  static int const F8{297};
  static int const F9{298};
  static int const F10{299};
  static int const F11{300};
  static int const F12{301};
  static int const F13{302};
  static int const F14{303};
  static int const F15{304};
  static int const F16{305};
  static int const F17{306};
  static int const F18{307};
  static int const F19{308};
  static int const F20{309};
  static int const F21{310};
  static int const F22{311};
  static int const F23{312};
  static int const F24{313};
  static int const F25{314};
  static int const Kp0{320};
  static int const Kp1{321};
  static int const Kp2{322};
  static int const Kp3{323};
  static int const Kp4{324};
  static int const Kp5{325};
  static int const Kp6{326};
  static int const Kp7{327};
  static int const Kp8{328};
  static int const Kp9{329};
  static int const KpDecimal{330};
  static int const KpDivide{331};
  static int const KpMultiply{332};
  static int const KpSubtract{333};
  static int const KpAdd{334};
  static int const KpEnter{335};
  static int const KpEqual{336};
  static int const LeftShift{340};
  static int const LeftCtrl{341};
  static int const LeftAlt{342};
  static int const LeftSuper{343};
  static int const RightShift{344};
  static int const RightCtrl{345};
  static int const RightAlt{346};
  static int const RightSuper{347};
  static int const Menu{348};

  static int const Last{Menu};
};

class IO {

  // * WINDOW

public:
  bool update();
  void close();

  bool  isValid() const;
  bool  isFocused() const;
  float getAspectRatio() const;

  std::shared_ptr<Scene> getActiveScene() const;

private:
  static inline unsigned int   windowCounter = 0;
  static constexpr float       WIDTH         = 800.f;
  static constexpr float       HEIGHT        = 600.f;
  static constexpr const char *TITLE         = "VONSAI";

  bool m_valid{false};
  bool m_focused{false};

  void *m_window{nullptr};
  void *m_gui{nullptr};

  std::string          m_title{TITLE};
  float                m_width{WIDTH};
  float                m_height{HEIGHT};
  std::array<float, 3> m_color{0.13f, 0.13f, 0.175f};

  std::list<std::shared_ptr<Scene>> m_scenes;
  std::shared_ptr<Scene>            m_activeScene;

  void destroy();
  void activate();

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

private:
  // TODO : Improve friend mechanism
  // * ( https://en.wikibooks.org/wiki/More_C++_Idioms/Friendship_and_the_Attorney-Client )
  friend class Engine;

  explicit IO(uint16_t a_width, uint16_t a_height); // Factory pattern from Engine class
  static void shutdown();
};

} // namespace Vonsai
