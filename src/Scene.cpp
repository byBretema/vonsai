#include <Vonsai/Scene.hpp>
#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Wraps/_glm.hpp>

namespace Vonsai {

void Scene::update() {
  //! Just draw, do not worry about swap buffer or glClear.
  onRender();
}

} // namespace Vonsai
