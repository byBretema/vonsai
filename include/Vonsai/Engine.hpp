#pragma once

#include <list>
#include <memory>

#include "IO.hpp"
#include "Renderable.hpp"
#include "Shader.hpp"

namespace Vonsai {

class Engine {
public:
  static constexpr uint8_t MAX_ALLOWED_WINDOWS = 8;

  struct {
    std::unique_ptr<Shader> light;
    std::unique_ptr<Shader> flat;
  } shader;

  struct {
    std::unique_ptr<Renderable> monkey;
    std::unique_ptr<Renderable> cube;
    std::unique_ptr<Renderable> plane;
  } mesh;

  Engine();
  void run() const;

  std::shared_ptr<IO> getWindow(std::string const &a_name) const;
  std::shared_ptr<IO> addWindow(std::string const &a_name, uint16_t a_width, uint16_t a_height);

private:
  // std::vector<Shader *>                                        m_shaders;
  // std::vector<Renderable *>                                    m_renderables;
  mutable std::unordered_map<std::string, std::shared_ptr<IO>> m_ios;
};

} // namespace Vonsai
