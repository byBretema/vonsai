#include <Vonsai/Context.hpp>

#include <Vonsai/Renderable.hpp>
#include <Vonsai/Scene.hpp>
#include <Vonsai/Shader.hpp>

#include <Vonsai/Camera.hpp>
#include <Vonsai/Light.hpp>

#include <Vonsai/Utils/Strings.hpp>
#include <Vonsai/Wraps/_globals.hpp>

namespace Vonsai {

//---------------------------------------------------------------------------------------------------------------------

Context::Context() {

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  // for (auto i = 0u; i < (unsigned int)EScene::LAST; ++i) { m_scenes.emplace((EScene)i, Scene{}); }
  m_scenes.emplace(EScene::S0, Scene{});

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  m_meshes.emplace(EMesh::GLORIES, Mesh::import("/Users/cambalamas/Desktop/Glories/HighPoly/GloriesTower_HP.obj"));
  // m_meshes.emplace(EMesh::MONKEY, Mesh::import(vo_res + "models/monkey.obj"));
  // m_meshes.emplace(EMesh::CUBE, Mesh::import(vo_res + "models/cube.fbx"));
  // m_meshes.emplace(EMesh::PLANE, Mesh::import(vo_res + "models/plane.obj"));
  // m_meshes.emplace(EMesh::DRAGON, Mesh::import(vo_res + "models/dragon.obj"));
  // m_meshes.emplace(EMesh::BODY, Mesh::import(vo_res + "models/kenney/character.fbx"));
  // m_meshes[(int)EMesh::BODY]->transform(0u)->modRotX(-90.f);
  // m_meshes[(int)EMesh::BODY]->transform(0u)->modPosY(-1.75f);

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  m_shaders.emplace(EShader::LIGHT, Shader("light", ShaderPath{vo_res + "shaders/light/light.vert", //
                                                               vo_res + "shaders/light/light.frag", //
                                                               vo_res + "shaders/light/light.geom"}));

  m_shaders.emplace(EShader::NORMALS, Shader("normals", ShaderPath{vo_res + "shaders/normals/normals.vert", //
                                                                   vo_res + "shaders/normals/normals.frag", //
                                                                   vo_res + "shaders/normals/normals.geom"}));

  m_shaders.emplace(EShader::DEBUG, Shader("debug", ShaderPath{vo_res + "shaders/debug/debug.vert", //
                                                               vo_res + "shaders/debug/debug.frag", //
                                                               vo_res + "shaders/debug/debug.geom"}));

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
} // namespace Vonsai

//---------------------------------------------------------------------------------------------------------------------

void Context::linkUBO(std::string const &name, int bindPoint) {
  for (auto &&s : m_shaders) s.second.linkUBO(name, bindPoint);
}

//---------------------------------------------------------------------------------------------------------------------

UBO Context::linkLightUBO([[maybe_unused]] std::string const &name, std::vector<Light> const &lights) {
  UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lights.size())});
  lightsUBO.setData("u_lights", lights);
  return lightsUBO;
}

//---------------------------------------------------------------------------------------------------------------------

UBO Context::linkCameraUBO([[maybe_unused]] std::string const &name, [[maybe_unused]] Camera const &camera) {
  UBO cameraUBO;
  linkUBO("camera", cameraUBO.getBindPoint());
  return cameraUBO;
}

//---------------------------------------------------------------------------------------------------------------------

Shader &               Context::get(EShader idx) { return m_shaders.at(idx); }
RenderableGroup &      Context::get(EMesh idx) { return m_meshes.at(idx); }
Scene &                Context::get(EScene idx) { return m_scenes.at(idx); }
Shader const &         Context::getc(EShader idx) { return m_shaders.at(idx); }
RenderableGroup const &Context::getc(EMesh idx) { return m_meshes.at(idx); }
Scene const &          Context::getc(EScene idx) { return m_scenes.at(idx); }

//---------------------------------------------------------------------------------------------------------------------

Input const &Context::getInput() const { return m_input; }
float        Context::getAspectRatio() const { return m_window.getAspectRatio(); }

//---------------------------------------------------------------------------------------------------------------------

void Context::run() const {
  if (m_scenes.size() < 1) { return; }
  EScene ES = (EScene)m_currSceneIdx;

  while (update(m_scenes.at(ES))) {
    m_input.resetScrollAndAxis();
    if (m_input.keyHold(KeyCode::Esc)) { m_window.close(); }
    if (m_input.keyPress(KeyCode::Right)) { m_currSceneIdx = ++m_currSceneIdx % m_scenes.size(); }
    if (m_input.keyPress(KeyCode::Left)) { m_currSceneIdx = --m_currSceneIdx % m_scenes.size(); }
  }
  m_window.shutdown();
}

//---------------------------------------------------------------------------------------------------------------------

bool Context::update(Scene const &a_scene) const {
  return m_window.update(a_scene.getOnUpdateFn(), a_scene.getOnGuiFn());
}

//---------------------------------------------------------------------------------------------------------------------

} // namespace Vonsai
