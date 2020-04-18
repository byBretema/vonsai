// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <imgui/imgui.h>

#include <Vonsai/Vonsai.hpp>
using Light   = vo::Light;
using Texture = vo::Texture;
using UBO     = vo::UBO;
using Camera  = vo::Camera;
using KC      = vo::KeyCode;


int main() {
  Vonsai::Engine vo;
  auto           IO = vo.getWindow("default");
  auto           SC = IO->getActiveScene();

  // ========================================================================================== //

  Light              l1({0, 2, 5}, {1, 1, 1});
  Light              l2({0, 2, -5}, {1, 1, 1});
  std::vector<Light> lv{l1, l2};

  UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  vo.shader.light->setUniformBlock("lights", lightsUBO.getBindPoint());

  UBO cameraUBO;
  vo.shader.light->setUniformBlock("camera", cameraUBO.getBindPoint());

  Texture tex1("assets/textures/dac.png");
  Texture tex2("assets/textures/Vonsai.png");
  Texture tex3("assets/textures/chess.jpg");

  Camera camera;
  camera.setZoom(17.5f);

  // ========================================================================================== //

  SC->setClearColor(0.6, 0.3, 0.1);
  SC->onUpdate = [&]() {
    camera.defaultBehaviour(cameraUBO, IO);

    vo.mesh.monkey->transform.setPosX(-3.f);
    vo.mesh.monkey->draw(*vo.shader.light, camera.getView(), &tex1);
    vo.mesh.monkey->transform.setPosX(0.f);
    vo.mesh.monkey->draw(*vo.shader.light, camera.getView(), &tex2);
    vo.mesh.monkey->transform.setPosX(3.f);
    vo.mesh.monkey->draw(*vo.shader.light, camera.getView(), &tex3);

    vo.mesh.plane->transform.reset();
    vo.mesh.plane->draw(*vo.shader.light, camera.getView());
  };

  SC->onGui = []() { ImGui::Text("hell0!"); };

  vo.run();
}
