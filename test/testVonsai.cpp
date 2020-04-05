// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <Vonsai/App.hpp>
#include <Vonsai/Light.hpp>
#include <Vonsai/Texture.hpp>
#include <Vonsai/Utils/Colors.hpp>

int main() {
  Vonsai::App app;

  Vonsai::Light              l1({0, 2, 5}, {1, 1, 1});
  Vonsai::Light              l2({0, 2, -5}, {1, 1, 1});
  std::vector<Vonsai::Light> lv{l1, l2};

  Vonsai::UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  app.shader.light->setUniformBlock("lightsUBO", lightsUBO.getBindPoint());

  Vonsai::UBO globalData;
  app.shader.light->setUniformBlock("globalData", globalData.getBindPoint());

  // ========================================================================================== //

  Vonsai::Texture tex1("assets/textures/dac.png");
  Vonsai::Texture tex2("assets/textures/Vonsai.png");
  Vonsai::Texture tex3("assets/textures/chess.jpg");

  // ========================================================================================== //

  app.m_window->camera.pivot.pos += glm::vec3{0, 0, 17.5f};

  glm::vec3 midMonkeyPos{0.f};

  app.m_window->setRenderFn([&]() {
    app.m_window->camera.frame(app.m_window->getAspectRatio());

    globalData.setData("u_proj", app.m_window->camera.getProj());
    globalData.setData("u_view", app.m_window->camera.getView());

    app.mesh.monkey->transform.pos = {-3, 0, 0};
    app.mesh.monkey->draw(*app.shader.light, app.m_window->camera.getView(), &tex1);
    app.mesh.monkey->transform.pos = midMonkeyPos;
    app.mesh.monkey->draw(*app.shader.light, app.m_window->camera.getView(), &tex2);
    app.mesh.monkey->transform.pos = {3, 0, 0};
    app.mesh.monkey->draw(*app.shader.light, app.m_window->camera.getView(), &tex3);
  });

  app.m_window->setInputFn([&midMonkeyPos](int a_mod, Vonsai::Window &a_window) {
    if (a_window.getKey(GLFW_KEY_R)) { a_window.setClearColor(Vonsai::Colors::random()); }
    if (a_window.getKey(GLFW_KEY_I)) { midMonkeyPos.z += 0.1f; }
    if (a_window.getKey(GLFW_KEY_K)) { midMonkeyPos.z -= 0.1f; }
    if (a_window.getKey(GLFW_KEY_L)) { midMonkeyPos.x += 0.1f; }
    if (a_window.getKey(GLFW_KEY_J)) { midMonkeyPos.x -= 0.1f; }
  });

  Vonsai::Window::run();
}
