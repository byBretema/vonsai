#pragma once

#include <string>

namespace Vonsai {

class Input;
class Window {
public:
  Window(Input *a_input, unsigned int a_width, unsigned int a_height);

  bool  isValid() const;
  bool  isFocused() const;
  float getAspectRatio() const;

  bool update(std::function<unsigned int(void)> const &a_onUpdate, std::function<void(void)> const &a_onGui);
  void close();
  void shutdown();

private:
  static inline unsigned int   s_counter{0u};
  static constexpr const char *DEFAULT_TITLE{"VONSAI"};

  bool m_valid{false};
  bool m_focused{false};

  void *m_window{nullptr};
  void *m_gui{nullptr};

  std::string m_title{DEFAULT_TITLE};
  float       m_width{800.f};
  float       m_height{600.f};

  void activate();
  void destroy();

  friend void onWindowDestroy(Window &a_window);
  friend void onWindowFocus(bool a_isFocused, Window &a_window);
  friend void onWindowResize(float a_width, float a_height, Window &a_window);
};

} // namespace Vonsai
