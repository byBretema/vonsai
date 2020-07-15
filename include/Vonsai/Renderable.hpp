#pragma once

#include "Bindable.hpp"
#include "Transform.hpp"
#include "Wraps/_glm.hpp"

#include <unordered_map>
#include <vector>

namespace Vonsai {

// --------------------------------------------------------------------------------------------------------------------

struct RenderablePOD {
  std::vector<unsigned int> indices{};
  std::vector<glm::vec3>    positions{};
  std::vector<glm::vec3>    normals{};
  std::vector<glm::vec2>    texCoords{};
  std::vector<glm::vec3>    tangents{};
  std::vector<glm::vec3>    bitangents{};
};

// --------------------------------------------------------------------------------------------------------------------

class Renderable : public Bindable {
public:
  Transform transform;

  explicit Renderable(std::string const &name, RenderablePOD const &a_data);

  void draw() const;
  bool isValid() const;

  void setEBO(const std::vector<unsigned int> &a_data);
  void addVBO(const std::vector<glm::vec3> &a_data);
  void addVBO(const std::vector<glm::vec2> &a_data);
  void addVBO(const std::vector<float> &a_data, int a_dataSize);

  // Not copiable
  Renderable(Renderable const &rhs) = delete;
  Renderable &operator=(Renderable const &rhs) = delete;

  // Move semantics
  Renderable(Renderable &&rhs) noexcept;
  Renderable &operator=(Renderable &&rhs) noexcept;
  friend void swap(Renderable &lhs, Renderable &rhs) noexcept;

private:
  std::string                                                 m_name;
  static inline std::unordered_map<std::string, unsigned int> s_uniqueRenderableNames{};

  bool         m_valid{false};
  mutable bool m_alertOnceInvalid{false};
  mutable bool m_alertOnceDraw{false};

  mutable unsigned int m_location{0u};
  unsigned int         m_VAO{0u};
  unsigned int         m_EBO{0u};
  unsigned int         m_indexCount{0u};

  void bind() const override;
  void unbind() const override;
};

// --------------------------------------------------------------------------------------------------------------------

class RenderableGroup {
public:
  explicit RenderableGroup(std::string const &name, std::vector<RenderablePOD> podGroup);
  // void                     draw();
  Transform *              transform(unsigned int idx);
  std::vector<Renderable> &group();

  RenderableGroup(RenderableGroup &&) = delete;
  RenderableGroup &operator=(RenderableGroup &&) = delete;
  RenderableGroup(RenderableGroup const &)       = delete;
  RenderableGroup &operator=(RenderableGroup const &) = delete;

private:
  std::vector<Renderable> m_group{};
};

// --------------------------------------------------------------------------------------------------------------------

} // namespace Vonsai
