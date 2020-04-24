#pragma once

#include <list>
#include <memory>

#include "IO/Input.hpp"
#include "IO/Window.hpp"

#include "Renderable.hpp"
#include "Shader.hpp"

#include "Scene.hpp"

namespace Vonsai {

class Engine {
public:
  struct {
    std::unique_ptr<Shader> light;
    std::unique_ptr<Shader> normals;
  } shader;

  struct {
    std::unique_ptr<Renderable> monkey;
    std::unique_ptr<Renderable> cube;
    std::unique_ptr<Renderable> plane;
    std::unique_ptr<Renderable> chandelier;
  } mesh;

  Engine();
  void run();

  std::tuple<Input const &, Window const &> getIO();

  inline Scene &getActiveScene() { return m_scenes.at(m_activeSceneID); }

private:
  Input  m_input{};
  Window m_window{&m_input, 800, 600};

  unsigned int       m_activeSceneID{0u};
  std::vector<Scene> m_scenes; // TODO : reserve
};

} // namespace Vonsai
