// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <imgui/imgui.h>

#include <Vonsai/Vonsai.hpp>
using Light   = vo::Light;
using Texture = vo::Texture;
using UBO     = vo::UBO;
using Camera  = vo::Camera;
namespace KC  = vo::KeyCode;


int main() {
  Vonsai::Engine voe;
  auto [input, window] = voe.getIO();
  auto &sc             = voe.getActiveScene();

  // ========================================================================================== //

  Light              l1({0, 2, 5}, {1, 1, 1});
  Light              l2({0, 2, -5}, {1, 1, 1});
  std::vector<Light> lv{l1, l2};

  UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  voe.shader.light->setUniformBlock("lights", lightsUBO.getBindPoint());

  UBO cameraUBO;
  voe.shader.light->setUniformBlock("camera", cameraUBO.getBindPoint());

  Texture tex1("assets/textures/dac.png");
  Texture tex2("assets/textures/Vonsai.png");
  Texture tex3("assets/textures/chess.jpg");

  Camera camera;
  camera.setZoom(17.5f);

  // ========================================================================================== //

  sc.setOnUpdateFn([&, &input = input, &window = window]() {
    sc.setClearColor(0.6, 0.3, 0.1);
    camera.defaultBehaviour(sc.getDeltaTime(), window.getAspectRatio(), cameraUBO, input);

    voe.mesh.monkey->transform.setPosX(-3.f);
    voe.mesh.monkey->draw(*voe.shader.light, camera.getView(), &tex1);
    voe.mesh.monkey->transform.setPosX(0.f);
    voe.mesh.monkey->draw(*voe.shader.light, camera.getView(), &tex2);
    voe.mesh.monkey->transform.setPosX(3.f);
    voe.mesh.monkey->draw(*voe.shader.light, camera.getView(), &tex3);

    voe.mesh.plane->transform.reset();
    voe.mesh.plane->draw(*voe.shader.light, camera.getView());
  });

  sc.setOnGuiFn([]() { ImGui::Text("hell0!"); });

  voe.run();
  return 0;
}
