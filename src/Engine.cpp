#include <Vonsai/Engine.hpp>
#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Strings.hpp>

#include <Vonsai/MeshLoader.hpp>

namespace Vonsai {

Engine::Engine() {
  m_scenes.emplace_back();

  mesh.monkey     = std::make_unique<Renderable>(getMeshFromFile("assets/models/monkey.obj"));
  mesh.cube       = std::make_unique<Renderable>(getMeshFromFile("assets/models/cube.obj"));
  mesh.plane      = std::make_unique<Renderable>(getMeshFromFile("assets/models/plane.obj"));
  mesh.chandelier = std::make_unique<Renderable>(getMeshFromFile("assets/models/chandelier.obj"));

  Vonsai::ShaderPath lightSP;
  lightSP.vertex   = "assets/shaders/light/light.vert";
  lightSP.geometry = "assets/shaders/light/light.geom";
  lightSP.fragment = "assets/shaders/light/light.frag";
  shader.light     = std::make_unique<Shader>("light", lightSP);

  Vonsai::ShaderPath normalsSP;
  normalsSP.vertex   = "assets/shaders/normals/normals.vert";
  normalsSP.geometry = "assets/shaders/normals/normals.geom";
  normalsSP.fragment = "assets/shaders/normals/normals.frag";
  shader.normals     = std::make_unique<Shader>("normals", normalsSP);

  Vonsai::ShaderPath debugSP;
  debugSP.vertex   = "assets/shaders/debug/debug.vert";
  debugSP.geometry = "assets/shaders/debug/debug.geom";
  debugSP.fragment = "assets/shaders/debug/debug.frag";
  shader.debug     = std::make_unique<Shader>("debug", debugSP);
}

void Engine::run() {
  try {
    while (m_window.update(                                         //
        SceneAttorney::getOnUpdateFn(m_scenes.at(m_activeSceneID)), // TODO :
        SceneAttorney::getOnGuiFn(m_scenes.at(m_activeSceneID))))   // TODO :
    {
      m_input.resetScrollAndAxis();
      if (m_input.key(KeyCode::Esc)) { m_window.close(); }
    }
    // m_window.close();
    m_window.shutdown(); // ! needed for clean-up
  } catch (std::exception &e) {
    vo_err("EXCEPTION CAUGHT: {}", e.what()); //
  } catch (const char *msg) {
    vo_err("EXCEPTION CAUGHT: {}", msg); //
  } catch (...) {
    throw; // Re-throw the exception so OS gives you a debug opportunity.
  }
}

std::tuple<Input const &, Window const &> Engine::getIO() { return {m_input, m_window}; }

} // namespace Vonsai
