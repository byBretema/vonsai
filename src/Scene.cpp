#include <Vonsai/Scene.hpp>
#include <Vonsai/Utils/Logger.hpp>

#include <Vonsai/Wraps/_gl.hpp>
#include <Vonsai/Wraps/_glm.hpp>

namespace Vonsai {

unsigned int Scene::getFPS() { return m_exposedFrameCounter; }
float        Scene::getDeltaTime() { return m_deltaTime; }

void Scene::setOnGuiFn(std::function<void(void)> a_fn) { m_onGui = a_fn; }
void Scene::setOnUpdateFn(std::function<void(void)> a_fn) { m_onUpdate = a_fn; }

void Scene::setClearColor(float a_r, float a_g, float a_b) { GL::clearColor(a_r, a_g, a_b); }

void Scene::updateFPS() {
  ++m_internalFrameCounter;
  auto const  currTime = Clock::now();
  float const seconds  = std::chrono::duration<float>(currTime - m_frameCounterTimeStamp).count();

  if (seconds >= 1.0) {
    m_frameCounterTimeStamp = currTime;
    m_exposedFrameCounter   = m_internalFrameCounter;
    m_internalFrameCounter  = 0;
  }
}

void Scene::updateDeltaTime() {
  auto const currTime = Clock::now();
  m_deltaTime         = std::chrono::duration<float>(currTime - m_deltaTimeStamp).count();
  m_deltaTimeStamp    = currTime;
}


// ATTORNEY ( https://en.wikibooks.org/wiki/More_C++_Idioms/Friendship_and_the_Attorney-Client )

std::function<unsigned int(void)> const &SceneAttorney::getOnUpdateFn(Scene const &a_scene) {
  return a_scene.m_internalOnUpdate;
}
std::function<void(void)> const &SceneAttorney::getOnGuiFn(Scene const &a_scene) { return a_scene.m_internalOnGui; }

} // namespace Vonsai
