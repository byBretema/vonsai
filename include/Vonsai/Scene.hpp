#pragma once

#include <chrono>
#include <functional>


namespace Vonsai {
using RenderFn = std::function<void(void)>;

class Scene {
public:
  static inline RenderFn const NullRenderFn{[]() {}};

  RenderFn onRender{NullRenderFn};
  explicit Scene() = default;
  void update();

private:
};

} // namespace Vonsai
