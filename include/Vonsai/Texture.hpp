#pragma once

#include "Bindable.hpp"

#include <string>

namespace Vonsai {

class Texture : public Bindable {
private:
  unsigned int m_ID     = 0u;
  std::string  m_path   = "";
  int          m_bytes  = 0;
  int          m_width  = 0;
  int          m_height = 0;

public:
  explicit Texture(const char *path);
  Texture(Texture &&) = delete;
  Texture &operator=(Texture &&) = delete;
  Texture(Texture const &)       = delete;
  Texture &operator=(Texture const &) = delete;

  unsigned int inline getID() const { return m_ID; }

  void bind() const override;
  void unbind() const override;
};

} // namespace Vonsai
