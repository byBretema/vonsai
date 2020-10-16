#pragma once

#include "Utils/Mesh.hpp"

#include "IO/Input.hpp"
#include "IO/Window.hpp"

#include <string>

#include <Vonsai/UBO.hpp>

/* clang-format off */
enum class EShader : unsigned char { LIGHT, NORMALS, DEBUG, LAST };
enum class EMesh   : unsigned char { /* MONKEY, CUBE, PLANE, BODY, DRAGON, NANOSUIT, */ GLORIES, LAST };
enum class EScene  : unsigned char { S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, LAST };
/* clang-format on */

namespace Vonsai {

class Renderable;
class Shader;
class Scene;
class Light;
class Camera;

// =========================================================================

class Context {
public:
  explicit Context();

  // Link UBOs and other actions to execute over ALL shaders in this context
  void linkUBO(std::string const &name, int bindPoint);
  UBO  linkLightUBO(std::string const &name, std::vector<Light> const &lights);
  UBO  linkCameraUBO(std::string const &name, Camera const &camera);

  // Below we can return a derreferenced pointer (a reference) because
  // we know that exist a data for every 'index' in the respective 'enum' specific type
  Shader &               get(EShader idx);
  RenderableGroup &      get(EMesh idx);
  Scene &                get(EScene idx);
  Shader const &         getc(EShader idx);
  RenderableGroup const &getc(EMesh idx);
  Scene const &          getc(EScene idx);

  Input const &getInput() const;
  float        getAspectRatio() const;

  void run() const;

private:
  std::unordered_map<EShader, Shader>        m_shaders;
  std::unordered_map<EMesh, RenderableGroup> m_meshes;

  std::unordered_map<EScene, Scene> m_scenes;
  mutable unsigned int              m_currSceneIdx{0u};

  bool update(Scene const &a_scene) const;

  mutable Input  m_input{};                    // ! FIXME : error if make it 'static' but it should be it.
  mutable Window m_window{&m_input, 800, 600}; // ! FIXME : error if make it 'static' but it should be it.
};

} // namespace Vonsai
