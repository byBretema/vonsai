#include <Vonsai/Context.hpp>

#include <Vonsai/Renderable.hpp>
#include <Vonsai/Scene.hpp>
#include <Vonsai/Shader.hpp>

namespace Vonsai {

Context::Context() {
  m_shaders.resize(static_cast<size_t>(EShader::LAST));
  m_meshes.resize(static_cast<size_t>(EMesh::LAST));
  m_scenes.resize(static_cast<size_t>(EScene::LAST));

  static auto loadMesh = [&](EMesh idx, std::string const &path) {
    m_meshes[(int)idx] = new Renderable(Mesh::import(path).at(0));
  };

  loadMesh(EMesh::MONKEY, "assets/models/monkey.obj");
  loadMesh(EMesh::CUBE, "assets/models/cube.fbx");
  loadMesh(EMesh::PLANE, "assets/models/plane.obj");
  loadMesh(EMesh::DRAGON, "assets/models/dragon.obj");
  loadMesh(EMesh::BODY, "assets/models/kenney/character.fbx");
  m_meshes[(int)EMesh::BODY]->transform.modRotX(-90.f);
  m_meshes[(int)EMesh::BODY]->transform.modPosY(-1.75f);

  static auto loadShader = [&](EShader idx, std::string const &name, ShaderPath const &paths) {
    m_shaders[(int)idx] = new Shader(name, paths);
  };

  loadShader(EShader::LIGHT, "light",
             {"assets/shaders/light/light.vert", //
              "assets/shaders/light/light.frag", //
              "assets/shaders/light/light.geom"});

  loadShader(EShader::NORMALS, "normals",
             {"assets/shaders/normals/normals.vert", //
              "assets/shaders/normals/normals.frag", //
              "assets/shaders/normals/normals.geom"});

  loadShader(EShader::DEBUG, "debug",
             {"assets/shaders/debug/debug.vert", //
              "assets/shaders/debug/debug.frag", //
              "assets/shaders/debug/debug.geom"});
}

Context::~Context() {
  for (auto *p : m_shaders) { delete p; }
  for (auto *p : m_meshes) { delete p; }
}

void Context::linkUBO(std::string const &name, int bindPoint) {
  vo_log("LINKING UBO!!!!!");
  for (auto &&s : m_shaders) s->linkUBO(name, bindPoint);
}

Shader &Context::get(EShader idx) { return (idx == EShader::LAST) ? *m_shaders[(int)idx - 1] : *m_shaders[(int)idx]; }
Renderable &Context::get(EMesh idx) { return (idx == EMesh::LAST) ? *m_meshes[(int)idx - 1] : *m_meshes[(int)idx]; }
Scene &     Context::get(EScene idx) { return (idx == EScene::LAST) ? m_scenes[(int)idx - 1] : m_scenes[(int)idx]; }

Input const &Context::getInput() const { return m_input; }
float        Context::getAspectRatio() const { return m_window.getAspectRatio(); }

void Context::run() const {
  if (m_scenes.size() < 1) { return; }

  while (update(m_scenes.at(m_currSceneIdx))) {
    m_input.resetScrollAndAxis();
    if (m_input.keyHold(KeyCode::Esc)) { m_window.close(); }
    if (m_input.keyPress(KeyCode::Right)) { m_currSceneIdx = ++m_currSceneIdx % m_scenes.size(); }
    if (m_input.keyPress(KeyCode::Left)) { m_currSceneIdx = --m_currSceneIdx % m_scenes.size(); }
  }
  m_window.shutdown();
}

bool Context::update(Scene const &a_scene) const {
  return m_window.update(a_scene.getOnUpdateFn(), a_scene.getOnGuiFn());
}

} // namespace Vonsai
