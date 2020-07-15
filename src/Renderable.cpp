#include <Vonsai/Renderable.hpp>

#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Wraps/_gl.hpp>

#include <mutex>

namespace Vonsai {

// --------------------------------------------------------------------------------------------------------------------

Renderable::Renderable(std::string const &name, RenderablePOD const &a_data) {
  m_name = name;
  if (s_uniqueRenderableNames.count(name) > 0) {
    ++s_uniqueRenderableNames.at(name);
    m_name += "_" + std::to_string(s_uniqueRenderableNames.at(name));
  } else {
    s_uniqueRenderableNames.emplace(name, 0);
  }

  if (a_data.indices.size() < 3 or a_data.positions.size() < 3) { return; }

  GL_ASSERT(glGenVertexArrays(1, &m_VAO));
  setEBO(a_data.indices);
  addVBO(a_data.positions); // 0

  std::vector<glm::vec3> dummy3(a_data.indices.size(), {0, 0, 0});
  std::vector<glm::vec2> dummy2(a_data.indices.size(), {0, 0});
  // Safe empty cases
  (a_data.normals.size()) ? addVBO(a_data.normals) : addVBO(dummy3);       // 1
  (a_data.texCoords.size()) ? addVBO(a_data.texCoords) : addVBO(dummy2);   // 2
  (a_data.tangents.size()) ? addVBO(a_data.tangents) : addVBO(dummy3);     // 3
  (a_data.bitangents.size()) ? addVBO(a_data.bitangents) : addVBO(dummy3); // 4

  m_valid      = true;
  m_indexCount = a_data.indices.size();
}

// --------------------------------------------------------------------------------------------------------------------

void Renderable::draw() const {
  if (!m_alertOnceInvalid and !m_valid) {
    vo_err("Renderable '{}' : Something wrong in creation", m_name);
    m_alertOnceInvalid = true;
  } else if (!m_alertOnceDraw and m_indexCount < 3) {
    vo_err("Renderable '{}' : Not enough data to draw", m_name);
    m_alertOnceDraw = true;
  } else {
    BindGuard bgR{*this};
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
  }
}

// --------------------------------------------------------------------------------------------------------------------

bool Renderable::isValid() const { return m_valid; }

// --------------------------------------------------------------------------------------------------------------------

void Renderable::setEBO(std::vector<unsigned int> const &a_data) {
  if (m_indexCount) {
    vo_err("Renderable EBO Data only can be setted one time");
    return;
  }

  GL_ASSERT(glBindVertexArray(m_VAO));
  {
    GL_ASSERT(glGenBuffers(1, &m_EBO));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_data.size() * sizeof(a_data[0]), &a_data[0], GL_STATIC_DRAW));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  }
  GL_ASSERT(glBindVertexArray(0));
}

// --------------------------------------------------------------------------------------------------------------------

#define d_addVBO(a_data, a_dataSize, a_type)                                                               \
  BindGuard    bg{*this};                                                                                  \
  unsigned int m_VBO;                                                                                      \
  GL_ASSERT(glGenBuffers(1, &m_VBO));                                                                      \
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));                                                         \
  GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, a_data.size() * sizeof(a_data[0]), &a_data[0], GL_STATIC_DRAW)); \
  GL_ASSERT(glEnableVertexAttribArray(m_location));                                                        \
  GL_ASSERT(glVertexAttribPointer(m_location, a_dataSize, a_type, GL_FALSE, 0, 0));                        \
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));                                                             \
  ++m_location;

void Renderable::addVBO(std::vector<float> const &a_data, int a_dataSize) { d_addVBO(a_data, a_dataSize, GL_FLOAT); }
void Renderable::addVBO(std::vector<glm::vec3> const &a_data) { d_addVBO(a_data, a_data[0].length(), GL_FLOAT); }
void Renderable::addVBO(std::vector<glm::vec2> const &a_data) { d_addVBO(a_data, a_data[0].length(), GL_FLOAT); }

// --------------------------------------------------------------------------------------------------------------------

void Renderable::bind() const {
  GL_ASSERT(glBindVertexArray(m_VAO));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
}

// --------------------------------------------------------------------------------------------------------------------

void Renderable::unbind() const {
  GL_ASSERT(glBindVertexArray(0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

// --------------------------------------------------------------------------------------------------------------------

Renderable::Renderable(Renderable &&rhs) noexcept { swap(*this, rhs); }

// --------------------------------------------------------------------------------------------------------------------

Renderable &Renderable::operator=(Renderable &&rhs) noexcept {
  swap(*this, rhs);
  return *this;
}

// --------------------------------------------------------------------------------------------------------------------

void swap(Renderable &lhs, Renderable &rhs) noexcept {
  using std::swap;
  swap(lhs.m_name, rhs.m_name);
  swap(lhs.m_valid, rhs.m_valid);
  swap(lhs.m_alertOnceInvalid, rhs.m_alertOnceInvalid);
  swap(lhs.m_alertOnceDraw, rhs.m_alertOnceDraw);
  swap(lhs.m_location, rhs.m_location);
  swap(lhs.m_VAO, rhs.m_VAO);
  swap(lhs.m_EBO, rhs.m_EBO);
  swap(lhs.m_indexCount, rhs.m_indexCount);
}

// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

RenderableGroup::RenderableGroup(std::string const &name, std::vector<RenderablePOD> podGroup) {
  int i = 0;
  for (auto &&mesh : podGroup) { m_group.emplace_back(name + "_" + std::to_string(++i), mesh); }
}

// --------------------------------------------------------------------------------------------------------------------

// void RenderableGroup::draw() {
//   for (auto &&renderable : m_group) { BindGuard bgR{renderable}; }
// }

// --------------------------------------------------------------------------------------------------------------------

Transform *RenderableGroup::transform(unsigned int idx) {
  return (idx < m_group.size()) ? &m_group.at(idx).transform : nullptr;
}

// --------------------------------------------------------------------------------------------------------------------

std::vector<Renderable> &RenderableGroup::group() { return m_group; }

} // namespace Vonsai
