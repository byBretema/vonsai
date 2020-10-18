#include <Vonsai/Texture.hpp>
#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Wraps/_gl.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Vonsai {

static int stbFlip = []() {
  stbi_set_flip_vertically_on_load(1);
  return 1;
}();

Texture::Texture(char const *path) {

  m_path = path;
  if (!Files::isValid(path, false)) { return; }

  // Parse image data
  auto pixels = stbi_load(path, &m_width, &m_height, &m_bytes, 4);

  if (!pixels) {
    vo_err("Failed loading texture: {}", path);
    return;
  }

  // Create opengl resource
  GL_ASSERT(glGenTextures(1, &m_ID));
  BindGuard bg{*this};

  // Attach image data to the texture
  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));

  // Free the pixels resource
  stbi_image_free(pixels);

  // MipMap generation and texture parameters
  GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);)
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);)
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);)
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);)
}

Texture::Texture(std::string const &path) : Texture(path.c_str()) {}

unsigned int Texture::getID() const { return m_ID; }

void Texture::bind() const {
  GL_ASSERT(glActiveTexture(GL_TEXTURE0 + m_ID));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_ID));
}

void Texture::unbind() const {
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0));
  GL_ASSERT(glActiveTexture(GL_TEXTURE0));
}

void swap(Texture &lhs, Texture &rhs) noexcept {
  using std::swap;
  swap(lhs.m_ID, rhs.m_ID);
  swap(lhs.m_path, rhs.m_path);
  swap(lhs.m_bytes, rhs.m_bytes);
  swap(lhs.m_width, rhs.m_width);
  swap(lhs.m_height, rhs.m_height);
}

} // namespace Vonsai
