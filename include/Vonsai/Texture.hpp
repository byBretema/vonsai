#pragma once

#include "Bindable.hpp"

#include <string>

namespace Vonsai {

class Texture : public Bindable {
private:
  unsigned int ID     = 0u;
  std::string  path   = "";
  int          bytes  = 0;
  int          width  = 0;
  int          height = 0;

public:
  explicit Texture(const char *path);
  Texture(Texture &&) = delete;
  Texture &operator=(Texture &&) = delete;
  Texture(Texture const &)       = delete;
  Texture &operator=(Texture const &) = delete;

  unsigned int inline getID() const { return ID; }

  void bind() const override;
  void unbind() const override;
};

} // namespace Vonsai
