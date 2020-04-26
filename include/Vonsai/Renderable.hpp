#pragma once

#include "Bindable.hpp"
#include "Transform.hpp"
#include "Wraps/_glm.hpp"

#include <vector>

namespace Vonsai {

struct RenderablePOD {
  std::vector<glm::vec3>    vertices{};
  std::vector<glm::vec3>    normals{};
  std::vector<glm::vec2>    texCoords{};
  std::vector<glm::vec3>    tangents{};
  std::vector<glm::vec3>    bitangents{};
  std::vector<unsigned int> indices{};
};

class Renderable : public Bindable {
public:
  Transform transform;

  explicit Renderable(RenderablePOD const &a_data);

  void draw() const;
  bool isValid() const;

  void setEBO(const std::vector<unsigned int> &a_data);
  void addVBO(const std::vector<glm::vec3> &a_data);
  void addVBO(const std::vector<glm::vec2> &a_data);
  void addVBO(const std::vector<float> &a_data, int a_dataSize);

  // Renderable(Renderable &&) = delete;
  // Renderable &operator=(Renderable &&) = delete;
  // Renderable(Renderable const &)       = delete;
  // Renderable &operator=(Renderable const &) = delete;

private:
  bool m_valid{false};

  mutable unsigned int m_location{0u};
  unsigned int         m_VAO{0u};
  unsigned int         m_EBO{0u};
  unsigned int         m_indexCount{0u};

  void bind() const override;
  void unbind() const override;
};

} // namespace Vonsai
