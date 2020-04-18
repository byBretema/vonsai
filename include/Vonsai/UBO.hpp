#pragma once

#include "Bindable.hpp"

#include "Wraps/_gl.hpp"
#include "Wraps/_glm.hpp"

#include <unordered_map>


/*
    THE ORDER IS CRUCIAL. (e.g. If you move one-line down 'mat4 view', in shader and don't do it on C/C++, 'view' will store the data of 'proj' and vice versa.) Also take into account that for correct align data inside the UBO should be multiple of vec4 or use an offest to avoid data corruption. (e.g. If you put first a 'vec3 color' you need to add a 'float color_offset' just before the 'mat4 view'.)

    ON SHADER:
      layout(std140) uniform globalData {
        vec3 color;
        float color_offest;
        mat4 proj;
        mat4 view;
      };

    ON C/C++:
      ubo.setData("color", glm::vec3{1.f, 0.f, 1.f});
      ubo.setData("view", cam->getView());
      ubo.setData("proj", cam->getProj());
  */


namespace Vonsai {

class UBO : public Bindable {
private:
  static inline unsigned int s_BindPoint{0u};

  unsigned int m_ID{0u};
  unsigned int m_bindPoint{s_BindPoint++};

  std::unordered_map<const char *, unsigned int> m_itemsOffset;
  unsigned int                                   m_totalSize{0u};

  void internalSetData(char const *a_name, size_t a_dataSize, void *a_dataDir);

public:
  UBO();
  UBO(UBO &&)  = delete;
  UBO &operator=(UBO &&) = delete;
  UBO(UBO const &)       = delete;
  UBO &operator=(UBO const &) = delete;

  // Injet float
  void setData(char const *a_name, float a_data);
  // Injet a GLM Matrix
  template <int NR, int NC, typename T, glm::qualifier Q>
  void setData(char const *a_name, glm::mat<NR, NC, T, Q> a_data) {
    internalSetData(a_name, sizeof(a_data), (void *)&a_data[0]);
  }
  // Injet a GLM Vector
  template <int N, typename T, glm::qualifier Q>
  void setData(char const *a_name, glm::vec<N, T, Q> a_data) {
    internalSetData(a_name, sizeof(a_data), (void *)&a_data[0]);
  }
  // Injet a Vector of any type
  template <typename T>
  void setData(char const *a_name, std::vector<T> const &a_data) {
    internalSetData(a_name, (a_data.size() * sizeof(a_data[0])), (void *)&a_data[0]);
  }

  void bind() const override;
  void unbind() const override;

  unsigned int inline getBindPoint() const { return m_bindPoint; }
};

} // namespace Vonsai
