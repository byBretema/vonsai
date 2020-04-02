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
  for (auto &&elem : positions) { vo_print("{},{},{}\n", elem.x, elem.y, elem.z); }
  vo_print("\nNORMALS\n");
  for (auto &&elem : normals) { vo_print("{},{},{}\n", elem.x, elem.y, elem.z); }
  vo_print("\nTEX-COORD\n");
  for (auto &&elem : texCoords) { vo_print("{},{}\n", elem.x, elem.y); }
  vo_print("\n");
}

void Renderable::bind() const {
  GL_ASSERT(glBindVertexArray(VAO));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
}
void Renderable::unbind() const {
  GL_ASSERT(glBindVertexArray(0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

Renderable::Renderable() { GL_ASSERT(glGenVertexArrays(1, &VAO)); }

Renderable::Renderable(RenderableData const &data) : Renderable() { fillFromData(data); }

void Renderable::fillFromData(RenderableData const &data) {
  setEBO(data.indices);
  addVBO(data.positions); // 0
  addVBO(data.normals);   // 1
  addVBO(data.texCoords); // 2

  indices   = data.indices;
  positions = data.positions;
  normals   = data.normals;
  texCoords = data.texCoords;
}

void Renderable::setEBO(std::vector<unsigned int> const &data) {
  if (!indices.empty()) {
    vo_err("Renderable EBO Data only can be setted one time");
    return;
  }

  GL_ASSERT(glBindVertexArray(VAO));
  {
    GL_ASSERT(glGenBuffers(1, &EBO));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data.front(), GL_STATIC_DRAW));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  }
  GL_ASSERT(glBindVertexArray(0));
}

#define d_addVBO(a_data, a_dataSize, a_type)                                                               \
  BindGuard    bg{*this};                                                                                  \
  unsigned int VBO;                                                                                        \
  GL_ASSERT(glGenBuffers(1, &VBO));                                                                        \
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, VBO));                                                           \
  GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, a_data.size() * sizeof(a_data[0]), &a_data[0], GL_STATIC_DRAW)); \
  GL_ASSERT(glEnableVertexAttribArray(location));                                                          \
  GL_ASSERT(glVertexAttribPointer(location, a_dataSize, a_type, GL_FALSE, 0, 0));                          \
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));                                                             \
  ++location;

void Renderable::addVBO(std::vector<float> const &data, int dataSize) { d_addVBO(data, dataSize, GL_FLOAT); }
void Renderable::addVBO(std::vector<glm::vec3> const &data) { d_addVBO(data, data[0].length(), GL_FLOAT); }
void Renderable::addVBO(std::vector<glm::vec2> const &data) { d_addVBO(data, data[0].length(), GL_FLOAT); }

void Renderable::draw() const {
  static std::once_flag alertOnceDraw;
  if (indices.size() < 3) {
    std::call_once(alertOnceDraw, [&] { vo_err("Not enough data to draw {} renderable", VAO); });
    return;
  }
  BindGuard r{*this};
  GL_ASSERT(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
}

} // namespace Vonsai
