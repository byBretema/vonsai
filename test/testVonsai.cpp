// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

// #include <Vonsai/App.hpp>
// #include <Vonsai/Light.hpp>
// #include <Vonsai/Texture.hpp>
// #include <Vonsai/Utils/Colors.hpp>

#include <Vonsai/Engine.hpp>
#include <Vonsai/Light.hpp>
// #include <Vonsai/UBO.hpp>
#include <list>


int main() {

  /*
  Vonsai::IO io1;
  Vonsai::IO io2;

  std::list<Vonsai::IO *> ios{&io1, &io2};
  while (ios.size() > 0) {
    for (auto io : ios) {
      io->update();
      if (io->key(Vonsai::KeyCode.Esc)) { io->close(); }
    }
    ios.remove_if([](auto io) { return !io->isValid(); });
  }
  */

  Vonsai::Engine vo;

  Vonsai::Light              l1({0, 2, 5}, {1, 1, 1});
  Vonsai::Light              l2({0, 2, -5}, {1, 1, 1});
  std::vector<Vonsai::Light> lv{l1, l2};

  Vonsai::UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  vo.shader.light->setUniformBlock("lightsUBO", lightsUBO.getBindPoint());

  Vonsai::UBO globalData;
  vo.shader.light->setUniformBlock("globalData", globalData.getBindPoint());

  // ========================================================================================== //

  Vonsai::Texture tex1("assets/textures/dac.png");
  Vonsai::Texture tex2("assets/textures/Vonsai.png");
  Vonsai::Texture tex3("assets/textures/chess.jpg");

  // ========================================================================================== //

  glm::mat4 mat{1.f};

  globalData.setData("u_proj", mat /* window.camera.getProj() */);
  globalData.setData("u_view", mat /* window.camera.getView() */);

  vo.m_ios.front()->m_activeScene->onRender = [&]() {
    vo.mesh.monkey->transform.pos = {-3, 0, 0};
    vo.mesh.monkey->draw(*vo.shader.light, mat /* window.camera.getView() */, &tex1);
    vo.mesh.monkey->transform.pos = {0, 0, 0};
    vo.mesh.monkey->draw(*vo.shader.light, mat /* window.camera.getView() */, &tex2);
    vo.mesh.monkey->transform.pos = {3, 0, 0};
    vo.mesh.monkey->draw(*vo.shader.light, mat /* window.camera.getView() */, &tex3);
  };

  vo.run();

  /*
  vo.addWindow("1", 800, 600);

  // Switch between scenes with TAB (by default) -- Responsibility of the engine
  vo.addScene("1", "Demo A", "#223");
  vo.addScene("1", "Demo B", "#232");

  vo.run([]() {
    // * Previous goes the iter from window list to get scenes and
    // * TAB key injection to allow switch between scenes
    // ! The code below gonna run: on-each ACTIVE Scene, of all windows.
    // system_a
    // system_b
  });

 */
  return 0;


  // Vonsai::App app;
  // auto        window = app.newWindow(800, 600, "VONSAI :: Window 1");

  // Vonsai::Light              l1({0, 2, 5}, {1, 1, 1});
  // Vonsai::Light              l2({0, 2, -5}, {1, 1, 1});
  // std::vector<Vonsai::Light> lv{l1, l2};

  // Vonsai::UBO lightsUBO;
  // lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  // lightsUBO.setData("u_lights", lv);
  // app.shader.light->setUniformBlock("lightsUBO", lightsUBO.getBindPoint());

  // Vonsai::UBO globalData;
  // app.shader.light->setUniformBlock("globalData", globalData.getBindPoint());

  // // ========================================================================================== //

  // Vonsai::Texture tex1("assets/textures/dac.png");
  // Vonsai::Texture tex2("assets/textures/Vonsai.png");
  // Vonsai::Texture tex3("assets/textures/chess.jpg");

  // // ========================================================================================== //

  // window.camera.pivot.pos += glm::vec3{0, 0, 17.5f};

  // glm::vec3 midMonkeyPos{0.f};

  // window.setRenderFn([&]() {
  //   window.camera.frame(window.getAspectRatio());

  //   globalData.setData("u_proj", window.camera.getProj());
  //   globalData.setData("u_view", window.camera.getView());

  //   app.mesh.monkey->transform.pos = {-3, 0, 0};
  //   app.mesh.monkey->draw(*app.shader.light, window.camera.getView(), &tex1);
  //   app.mesh.monkey->transform.pos = midMonkeyPos;
  //   app.mesh.monkey->draw(*app.shader.light, window.camera.getView(), &tex2);
  //   app.mesh.monkey->transform.pos = {3, 0, 0};
  //   app.mesh.monkey->draw(*app.shader.light, window.camera.getView(), &tex3);
  // });

  // window.setInputFn([&midMonkeyPos](int a_mod, Vonsai::Window &a_window) {
  //   if (a_window.getKey(GLFW_KEY_R)) { a_window.setClearColor(Vonsai::Colors::random()); }
  //   if (a_window.getKey(GLFW_KEY_I)) { midMonkeyPos.z += 0.1f; }
  //   if (a_window.getKey(GLFW_KEY_K)) { midMonkeyPos.z -= 0.1f; }
  //   if (a_window.getKey(GLFW_KEY_L)) { midMonkeyPos.x += 0.1f; }
  //   if (a_window.getKey(GLFW_KEY_J)) { midMonkeyPos.x -= 0.1f; }
  // });

  // Vonsai::Window::run();
}
