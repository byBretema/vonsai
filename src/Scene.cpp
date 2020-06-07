#include <Vonsai/Scene.hpp>
#include <Vonsai/Utils/Logger.hpp>

#include <Vonsai/Wraps/_gl.hpp>
#include <Vonsai/Wraps/_glm.hpp>

#include <imgui/imgui.h>

namespace Vonsai {

Scene::Scene()
    : m_internalOnGui([&]() {
        static bool showAlert{true};
        if (showAlert && (!m_onUpdate or !m_onGui)) {
          int const flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
          ImGui::Begin(">>> WARNING <<<", &showAlert, flags);
          if (!m_onUpdate) { ImGui::BulletText("SCENE IS EMPTY\t"); }
          if (!m_onGui) { ImGui::BulletText("GUI IS EMPTY\t"); }
          ImGui::End();
        }
        if (m_onGui) m_onGui();
      }),
      m_internalOnUpdate([&]() {
        updateFPS();
        updateDeltaTime();
        (m_onUpdate) ? m_onUpdate() : setClearColor(1.f, 0.f, 1.f);
        return m_exposedFrameCounter;
      }) {}

unsigned int Scene::getFPS() { return m_exposedFrameCounter; }
float        Scene::getDeltaTime() { return m_deltaTime; }

std::function<void(void)> const &        Scene::getOnGuiFn() const { return m_internalOnGui; }
std::function<unsigned int(void)> const &Scene::getOnUpdateFn() const { return m_internalOnUpdate; }

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

} // namespace Vonsai
