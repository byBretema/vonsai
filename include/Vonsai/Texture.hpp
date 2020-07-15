#pragma once
#include <utility>

#include "Bindable.hpp"

#include "Utils/Logger.hpp"

#include <string>

namespace Vonsai {

class Texture : public Bindable {
public:
  explicit Texture(const char *path);
  explicit Texture(std::string const &path);

  unsigned int getID() const;
  void         bind() const override;
  void         unbind() const override;


  friend void swap(Texture &lhs, Texture &rhs) noexcept;

  // Not copiable
  Texture(Texture const &) = delete;
  Texture &operator=(Texture const &) = delete;

  // Move semantics
  Texture(Texture &&rhs) noexcept { swap(*this, rhs); }
  Texture &operator=(Texture &&rhs) noexcept {
    swap(*this, rhs);
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
