#include <Vonsai/UBO.hpp>

#include <Vonsai/Wraps/_gl.hpp>

namespace Vonsai {

void UBO::bind() const { GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, ID)); }
void UBO::unbind() const { GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0)); }

UBO::UBO() {
  GL_ASSERT(glGenBuffers(1, &ID));
  bindPoint = gBindPoint++;
  GL_ASSERT(glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, ID));
}

#define d_UBO_setData(_dataSize, _dataDir)                                     \
  BindGuard uboBG{*this};                                                      \
  if (itemsOffset.count(name) < 1) {                                           \
    itemsOffset.emplace(name, totalSize);                                      \
    totalSize += _dataSize;                                                    \
    GL_ASSERT(glBufferData(GL_UNIFORM_BUFFER, totalSize, 0, GL_DYNAMIC_DRAW)); \
  }                                                                            \
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, itemsOffset[name], _dataSize, _dataDir));

void UBO::setData(char const *name, float data) { d_UBO_setData(sizeof(data), &data); }
void UBO::setData(char const *name, glm::vec2 data) { d_UBO_setData(sizeof(data), &data[0]); }
void UBO::setData(char const *name, glm::vec3 data) { d_UBO_setData(sizeof(data), &data[0]); }
void UBO::setData(char const *name, glm::mat4 data) { d_UBO_setData(sizeof(data), &data[0]); }

} // namespace Vonsai
