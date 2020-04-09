#pragma once

#include "IO.hpp"
#include "Scene.hpp"

#include "Renderable.hpp"
#include "Shader.hpp"

#include <memory>

namespace Vonsai {

class Engine {
public:
  // struct {
  //   std::shared_ptr<Shader> light;
  //   std::shared_ptr<Shader> flat;
  // } shader;

  // struct {
  //   std::shared_ptr<Renderable> monkey;
  //   std::shared_ptr<Renderable> cube;
  // } mesh;

  // Renderable &newRenderable(std::string const &a_name, std::string const &a_path);
  // Shader &    newShader(std::string const &a_name, ShaderPath const &a_path);
  // Shader &    newShader(std::string const &a_name, ShaderCode const &a_code);

  Engine();
  void addWindow(std::string const &a_name, int a_width, int a_height);
  void addScene(std::string const &a_windowName, std::string const &a_sceneName, std::string const &a_hexColor);

private:
  std::vector<IO>                      m_ioVec;
  std::unordered_map<std::string, int> m_ioTable;

  std::vector<Scene>                                                    m_sceneVec;
  std::unordered_map<std::string, std::unordered_map<std::string, int>> m_sceneTable;

  // std::vector<Renderable *> m_renderables;
  // std::vector<Shader *>     m_shaders;
  // std::vector<IO *>         m_windows;
};

} // namespace Vonsai
