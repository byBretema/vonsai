#pragma once

#include "Bindable.hpp"
#include "Transform.hpp"
#include "Wraps/_glm.hpp"

#include <vector>

namespace Vonsai {

struct RenderableData {
  std::vector<glm::vec3>    positions = {};
  std::vector<glm::vec3>    normals   = {};
  std::vector<glm::vec2>    texCoords = {};
  std::vector<unsigned int> indices   = {};

  void info() const;
};

class Renderable : public Bindable {
private:
  mutable unsigned int location = 0u;
  unsigned int         VAO = 0u, EBO = 0u;

  std::vector<glm::vec3>    positions = {};
  std::vector<glm::vec3>    normals   = {};
  std::vector<glm::vec2>    texCoords = {};
  std::vector<unsigned int> indices   = {};

public:
  Renderable();
  explicit Renderable(RenderableData const &data);
  Renderable(Renderable &&) = delete;
  Renderable &operator=(Renderable &&) = delete;
  Renderable(Renderable const &)       = delete;
  Renderable &operator=(Renderable const &) = delete;

  Transform transform;

  void fillFromData(RenderableData const &data);

  void setEBO(const std::vector<unsigned int> &data);

  void addVBO(const std::vector<float> &data, int dataSize);
  void addVBO(const std::vector<glm::vec3> &data);
  void addVBO(const std::vector<glm::vec2> &data);

  void draw() const;

  void bind() const override;
  void unbind() const override;
};

} // namespace Vonsai
