#include <Vonsai/Scene.hpp>
#include <Vonsai/Wraps/_glm.hpp>

namespace Vonsai {

void Scene::update() {}
void Scene::setClearColor(glm::vec3 const &a_ClearColor) {}
void Scene::setRenderFn(RenderFn a_func) { m_onRender = (!a_func) ? s_nullRenderFn : a_func; }

void Scene::updateFPS() {}

} // namespace Vonsai
