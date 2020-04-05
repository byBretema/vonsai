#pragma once

#include <Vonsai/Renderable.hpp>
#include <Vonsai/Shader.hpp>
#include <Vonsai/Window.hpp>

#include <memory>

namespace Vonsai {

class App {
public:
  std::unique_ptr<Window> m_window;

  struct {
    std::shared_ptr<Shader> light;
    std::shared_ptr<Shader> flat;
  } shader;

  struct {
    std::shared_ptr<Renderable> monkey;
    std::shared_ptr<Renderable> cube;
  } mesh;

  App();
};

} // namespace Vonsai
