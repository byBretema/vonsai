#pragma once

#include "Bindable.hpp"

#include "Utils/Logger.hpp"

#include <string>

namespace Vonsai {

class Texture : public Bindable {
public:
  explicit Texture(const char *path);

  unsigned int getID() const;
  void         bind() const override;
  void         unbind() const override;

  // TODO : Change this to a Copy-Swap idiom to be less verbose and error-prone

  Texture(Texture &&rhs) noexcept {
    if (this != &rhs) {
      m_ID     = rhs.m_ID;
      m_path   = rhs.m_path;
      m_bytes  = rhs.m_bytes;
      m_width  = rhs.m_width;
      m_height = rhs.m_height;
      // ---
      rhs.m_ID     = 0u;
      rhs.m_path   = "";
      rhs.m_bytes  = 0;
      rhs.m_width  = 0;
      rhs.m_height = 0;
    }
  }
  Texture &operator=(Texture &&rhs) noexcept {
    if (this != &rhs) {
      m_ID     = rhs.m_ID;
      m_path   = rhs.m_path;
      m_bytes  = rhs.m_bytes;
      m_width  = rhs.m_width;
      m_height = rhs.m_height;
      // ---
      rhs.m_ID     = 0u;
      rhs.m_path   = "";
      rhs.m_bytes  = 0;
      rhs.m_width  = 0;
      rhs.m_height = 0;
    }
    return *this;
  };
  Texture(Texture const &rhs) {
    if (this != &rhs) {
      m_ID     = rhs.m_ID;
      m_path   = rhs.m_path;
      m_bytes  = rhs.m_bytes;
      m_width  = rhs.m_width;
      m_height = rhs.m_height;
    }
  };
  Texture &operator=(Texture const &rhs) {
    if (this != &rhs) {
      m_ID     = rhs.m_ID;
      m_path   = rhs.m_path;
      m_bytes  = rhs.m_bytes;
      m_width  = rhs.m_width;
      m_height = rhs.m_height;
    }
    return *this;
  };

private:
  unsigned int m_ID     = 0u;
  std::string  m_path   = "";
  int          m_bytes  = 0;
  int          m_width  = 0;
  int          m_height = 0;
};

} // namespace Vonsai
