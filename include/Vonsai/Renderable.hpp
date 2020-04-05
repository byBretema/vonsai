#pragma once

#include "Bindable.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Wraps/_glm.hpp"

#include <vector>

namespace Vonsai {

struct RenderableData {
  std::vector<glm::vec3>    vertices{};
  std::vector<glm::vec3>    normals{};
  std::vector<glm::vec2>    texCoords{};
  std::vector<unsigned int> indices{};

  void info() const;
};

class Renderable : public Bindable {
private:
  bool m_valid = true;

  mutable unsigned int m_location{0u};
  unsigned int         m_VAO{0u};
  unsigned int         m_EBO{0u};

  std::vector<glm::vec3>    m_vertices{};
  std::vector<glm::vec3>    m_normals{};
  std::vector<glm::vec2>    m_texCoords{};
  std::vector<unsigned int> m_indices{};

  void bind() const override;
  void unbind() const override;

public:
  Transform transform;

  void draw(Shader const &a_shader, glm::mat4 const &a_view, Texture const *a_texture = nullptr) const;

  void setEBO(const std::vector<unsigned int> &a_data);
  void addVBO(const std::vector<glm::vec3> &a_data);
  void addVBO(const std::vector<glm::vec2> &a_data);
  void addVBO(const std::vector<float> &a_data, int a_dataSize);

  explicit Renderable(RenderableData const &a_data);
  Renderable(Renderable &&) = delete;
  Renderable &operator=(Renderable &&) = delete;
  Renderable(Renderable const &)       = delete;
  Renderable &operator=(Renderable const &) = delete;
};

} // namespace Vonsai
