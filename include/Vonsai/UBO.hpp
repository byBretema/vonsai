#pragma once

#include "Bindable.hpp"
#include "Wraps/_glm.hpp"

#include <unordered_map>

namespace Vonsai {

class UBO : public Bindable {
private:
  static inline unsigned int gBindPoint = 0u;

  unsigned int ID        = 0u;
  unsigned int bindPoint = 0u;

  std::unordered_map<const char *, unsigned int> itemsOffset;
  unsigned int                                   totalSize = 0u;

public:
  UBO();
  UBO(UBO &&)  = delete;
  UBO &operator=(UBO &&) = delete;
  UBO(UBO const &)       = delete;
  UBO &operator=(UBO const &) = delete;

  /*
    USING THIS, THE ORDER IS CRUCIAL. (e.g. If you move one-line down 'mat4 view', in shader and don't do it on C/C++, 'view' will store the data of 'proj' and vice versa.) Also take into account that for correct align data inside the UBO should be multiple of vec4 or use an offest to avoid data corruption. (e.g. If you put first a 'vec3 color' you need to add a 'float color_offset' just before the 'mat4 view'.)

    ON SHADER:
      layout(std140) uniform globalData {
        vec3 color;
        float color_offest;
        mat4 view;
        mat4 proj;
        mat4 viewproj;
      };

    ON C/C++:
      ubo.setData("color", glm::vec3{1.f, 0.f, 1.f});
      ubo.setData("view", cam->getView());
      ubo.setData("proj", cam->getProj());
      ubo.setData("viewproj", cam->getViewProj());
  */
  void setData(const char *name, float data);
  void setData(const char *name, glm::vec2 data);
  void setData(const char *name, glm::vec3 data);
  void setData(const char *name, glm::mat4 data);

  void bind() const override;
  void unbind() const override;

  unsigned int inline getBindPoint() const { return bindPoint; }
};

} // namespace Vonsai
