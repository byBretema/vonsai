#include <Vonsai/Scene.hpp>
#include <Vonsai/Utils/Logger.hpp>

#include <Vonsai/Wraps/_gl.hpp>
#include <Vonsai/Wraps/_glm.hpp>

namespace Vonsai {

uint16_t Scene::getFPS() { return m_exposedFrameCounter; }
float    Scene::getDeltaTime() { return m_deltaTime; }

void Scene::setClearColor(float r, float g, float b) { GL::clearColor(r, g, b); }

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

void Scene::internalOnGui() {
  if (onGui) onGui();
}

uint16_t Scene::internalOnUpdate() {
  updateFPS();
  updateDeltaTime();
  if (onUpdate) onUpdate();
  return m_exposedFrameCounter;
}

} // namespace Vonsai
