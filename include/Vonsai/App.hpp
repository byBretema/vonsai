#pragma once

#include <Vonsai/Renderable.hpp>
#include <Vonsai/Shader.hpp>
#include <Vonsai/Window.hpp>

#include <memory>

namespace Vonsai {

class App {
private:
  std::vector<Renderable *> m_renderables;
  std::vector<Shader *>     m_shaders;
  std::vector<Window *>     m_windows;

public:
  struct {
    std::shared_ptr<Shader> light;
    std::shared_ptr<Shader> flat;
  } shader;

  struct {
    std::shared_ptr<Renderable> monkey;
    std::shared_ptr<Renderable> cube;
  } mesh;

  App();
  Renderable &newRenderable(std::string const &a_name, std::string const &a_path);
  Shader &    newShader(std::string const &a_name, ShaderPath const &a_path);
  Shader &    newShader(std::string const &a_name, ShaderCode const &a_code);
  Window      newWindow(int a_width, int a_height, std::string const &a_name,
                        Window::InputFn const &a_inputFn = Window::s_nullInputFn);
};

} // namespace Vonsai
