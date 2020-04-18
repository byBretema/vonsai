#include <Vonsai/Renderable.hpp>

#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Wraps/_gl.hpp>

#include <mutex>

namespace Vonsai {

void RenderableData::info() const {
  vo_print("\nINDICES\n");
  {
    unsigned int count = 0u;
    for (auto &&elem : indices) {
      vo_print("{} ", elem);
      if (++count >= 3) {
        vo_print("\n");
        count = 0;
      }
    }
  }
  vo_print("\nPOSITION\n");
  for (auto &&elem : vertices) { vo_print("{},{},{}\n", elem.x, elem.y, elem.z); }
  vo_print("\nNORMALS\n");
  for (auto &&elem : normals) { vo_print("{},{},{}\n", elem.x, elem.y, elem.z); }
  vo_print("\nTEX-COORD\n");
  for (auto &&elem : texCoords) { vo_print("{},{}\n", elem.x, elem.y); }
  vo_print("\n");
}

void Renderable::bind() const {
  GL_ASSERT(glBindVertexArray(m_VAO));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
}
void Renderable::unbind() const {
  GL_ASSERT(glBindVertexArray(0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Renderable::draw(Shader const &a_shader, glm::mat4 const &a_view, Texture const *a_texture) const {
  static std::once_flag alertOnceShader, alertOnceInvalid, alertOnceDraw;
  if (!a_shader.isReady()) {
    std::call_once(alertOnceShader, [&] { vo_err("Corrupted shader: {}", a_shader.getName()); });
    return;
  }
  if (!m_valid) {
    std::call_once(alertOnceInvalid, [&] { vo_err("Something wrong in renderable {} creation", m_VAO); });
    return;
  }
  if (m_indices.size() < 3) {
    std::call_once(alertOnceDraw, [&] { vo_err("Not enough data to draw {} renderable", m_VAO); });
    return;
  }

  BindGuard bgR{*this};
  BindGuard bgS{a_shader};
  BindGuard bgT;

  if (a_texture) {
    bgT.inject(*a_texture);
    a_shader.setUniformInt1("u_texture", a_texture->getID());
  }

  auto const modelView = a_view * this->transform.matrix();
  a_shader.setUniformMat4("u_modelView", modelView);
  a_shader.setUniformMat4("u_normalMat", glm::transpose(glm::inverse(modelView)));

  GL_ASSERT(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0));
}

void Renderable::setEBO(std::vector<unsigned int> const &a_data) {
  if (!m_indices.empty()) {
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

Renderable::Renderable(RenderableData const &a_data) {
  if (a_data.indices.size() < 3 or a_data.vertices.size() < 3) {
    m_valid   = false;
    m_indices = {-1u};
    return;
  }
  GL_ASSERT(glGenVertexArrays(1, &m_VAO));
  setEBO(a_data.indices);
  addVBO(a_data.vertices);  // 0
  addVBO(a_data.normals);   // 1
  addVBO(a_data.texCoords); // 2
  m_indices   = a_data.indices;
  m_vertices  = a_data.vertices;
  m_normals   = a_data.normals;
  m_texCoords = a_data.texCoords;
}

} // namespace Vonsai
