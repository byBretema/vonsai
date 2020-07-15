#pragma once

#include "Utils/Mesh.hpp"

#include "IO/Input.hpp"
#include "IO/Window.hpp"

#include <string>

/* clang-format off */
enum class EShader : unsigned char { LIGHT, NORMALS, DEBUG, LAST };
enum class EMesh   : unsigned char { MONKEY, CUBE, PLANE, BODY, DRAGON, NANOSUIT, LAST };
enum class EScene  : unsigned char { S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, LAST };
/* clang-format on */

namespace Vonsai {

class Renderable;
class Shader;
class Scene;

// =========================================================================

class Context {
public:
  explicit Context();
  ~Context();

  // Link UBOs and other actions to execute over ALL shaders in this context
  // ! FIXME : Just not working
  void linkUBO(std::string const &name, int bindPoint);

  // Below we can return a derreferenced pointer (a reference) because
  // we know that exist a data for every 'index' in the respective 'enum' specific type
  Shader &         get(EShader idx);
  RenderableGroup &get(EMesh idx);
  Scene &          get(EScene idx);

  Input const &getInput() const;
  float        getAspectRatio() const;

  void run() const;

private:
  std::vector<Shader *>          m_shaders; // Must be pointers because Shader     doesn't have default-ctor.
  std::vector<RenderableGroup *> m_meshes;  // Must be pointers because Renderable doesn't have default-ctor.

  std::vector<Scene>   m_scenes;
  mutable unsigned int m_currSceneIdx = 0u;

  bool update(Scene const &a_scene) const;

  mutable Input  m_input{};                    // ! FIXME : error if make it 'static' but it should be it.
  mutable Window m_window{&m_input, 800, 600}; // ! FIXME : error if make it 'static' but it should be it.
};

} // namespace Vonsai
