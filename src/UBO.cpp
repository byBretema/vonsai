#include <Vonsai/UBO.hpp>

namespace Vonsai {

void UBO::bind() const { GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, m_ID)); }
void UBO::unbind() const { GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0)); }

void UBO::internalSetData(char const *a_name, size_t a_dataSize, void *a_dataDir) {
  BindGuard uboBG{*this};
  if (m_itemsOffset.count(a_name) < 1) {
    m_itemsOffset.emplace(a_name, m_totalSize);
    m_totalSize += a_dataSize;
    GL_ASSERT(glBufferData(GL_UNIFORM_BUFFER, m_totalSize, 0, GL_DYNAMIC_DRAW));
  }
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, m_itemsOffset[a_name], a_dataSize, a_dataDir));
}

UBO::UBO() {
  GL_ASSERT(glGenBuffers(1, &m_ID));
  GL_ASSERT(glBindBufferBase(GL_UNIFORM_BUFFER, m_bindPoint, m_ID));
}

} // namespace Vonsai
