#pragma once

#include "IO.hpp"
#include "Scene.hpp"

#include "Renderable.hpp"
#include "Shader.hpp"

#include <list>
#include <memory>

namespace Vonsai {

class Engine {
public:
  struct {
    std::unique_ptr<Shader> light;
    std::unique_ptr<Shader> flat;
  } shader;

  struct {
    std::unique_ptr<Renderable> monkey;
    std::unique_ptr<Renderable> cube;
  } mesh;

  // Renderable &newRenderable(std::string const &a_name, std::string const &a_path);
  // Shader &    newShader(std::string const &a_name, ShaderPath const &a_path);
  // Shader &    newShader(std::string const &a_name, ShaderCode const &a_code);

  Engine();
  void run();
  // void addWindow(std::string const &a_name, int a_width, int a_height);
  // void addScene(std::string const &a_windowName, std::string const &a_sceneName, std::string const &a_hexColor);

  std::list<std::unique_ptr<IO>> m_ios; // TODO: PRIVATE

private:
  using Clock     = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<Clock, std::chrono::nanoseconds>;

  double    m_deltaTime{0.f};
  TimePoint m_initTimeDelta{Clock::now()};
  TimePoint m_initTimeFrames{Clock::now()};

  std::vector<Shader *>     m_shaders;
  std::vector<Renderable *> m_renderables;
};

} // namespace Vonsai
