#include <Vonsai/Renderable.hpp>

#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Wraps/_gl.hpp>

#include <mutex>

namespace Vonsai {

Renderable::Renderable(RenderablePOD const &a_data) {
  if (a_data.indices.size() < 3 or a_data.vertices.size() < 3) { return; }
  GL_ASSERT(glGenVertexArrays(1, &m_VAO));
  setEBO(a_data.indices);
  addVBO(a_data.vertices);  // 0
  addVBO(a_data.normals);   // 1
  addVBO(a_data.texCoords); // 2

  m_valid      = true;
  m_indexCount = a_data.indices.size();
}

void Renderable::draw() const {
  static std::once_flag alertOnceInvalid, alertOnceDraw;
  if (!m_valid) {
    std::call_once(alertOnceInvalid, [&] { vo_err("Something wrong in renderable {} creation", m_VAO); });
  } else if (m_indexCount < 3) {
    std::call_once(alertOnceDraw, [&] { vo_err("Not enough data to draw {} renderable", m_VAO); });
  } else {
    BindGuard bgR{*this};
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
  }
}

bool Renderable::isValid() const { return m_valid; }

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

void Renderable::bind() const {
  GL_ASSERT(glBindVertexArray(m_VAO));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
}
void Renderable::unbind() const {
  GL_ASSERT(glBindVertexArray(0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

} // namespace Vonsai
