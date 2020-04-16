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
    std::shared_ptr<Shader> light;
    std::shared_ptr<Shader> flat;
  } shader;

  struct {
    std::shared_ptr<Renderable> monkey;
    std::shared_ptr<Renderable> cube;
  } mesh;

  Engine();
  void                run() const;
  std::shared_ptr<IO> getWindow(std::string const &a_name) const;

  // void addWindow(std::string const &a_name, int a_width, int a_height);
  // void addScene(std::string const &a_windowName, std::string const &a_sceneName, std::string const &a_hexColor);

private:
  using Clock     = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<Clock, std::chrono::nanoseconds>;

  // double    m_deltaTime{0.f};
  TimePoint m_initTimeDelta{Clock::now()};
  TimePoint m_initTimeFrames{Clock::now()};

  mutable std::unordered_map<std::string, std::shared_ptr<IO>> m_ios;

  std::vector<Shader *>     m_shaders;
  std::vector<Renderable *> m_renderables;
};

} // namespace Vonsai
