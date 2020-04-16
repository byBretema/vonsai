// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

// #include <Vonsai/App.hpp>
// #include <Vonsai/Light.hpp>
// #include <Vonsai/Texture.hpp>
// #include <Vonsai/Utils/Colors.hpp>

#include <Vonsai/Camera.hpp>
#include <Vonsai/Engine.hpp>
#include <Vonsai/Light.hpp>
#include <Vonsai/UBO.hpp>

#include <imgui/imgui.h>

#include <list>


int main() {

  Vonsai::Engine vo;
  auto           io = vo.getWindow("default");
  auto           sc = io->getActiveScene();
  auto &         kc = Vonsai::KeyCode;

  // ========================================================================================== //

  Vonsai::Light              l1({0, 2, 5}, {1, 1, 1});
  Vonsai::Light              l2({0, 2, -5}, {1, 1, 1});
  std::vector<Vonsai::Light> lv{l1, l2};

  Vonsai::UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  vo.shader.light->setUniformBlock("lightsUBO", lightsUBO.getBindPoint());

  Vonsai::UBO globalData;
  vo.shader.light->setUniformBlock("globalData", globalData.getBindPoint());

  Vonsai::Texture tex1("assets/textures/dac.png");
  Vonsai::Texture tex2("assets/textures/Vonsai.png");
  Vonsai::Texture tex3("assets/textures/chess.jpg");

  Vonsai::Camera camera;
  camera.setZoom(17.5f);

  // ========================================================================================== //

  sc->onRender = [&]() {
    // Camera update
    camera.frame(io->getAspectRatio());
    globalData.setData("u_proj", camera.getProj());
    globalData.setData("u_view", camera.getView());

    // Camera movement
    if (io->key(kc.E)) { camera.movement.U = true; } // Up
    if (!io->key(kc.E)) { camera.movement.U = false; }
    if (io->key(kc.Q)) { camera.movement.D = true; } // Down
    if (!io->key(kc.Q)) { camera.movement.D = false; }
    if (io->key(kc.W)) { camera.movement.F = true; } // Front
    if (!io->key(kc.W)) { camera.movement.F = false; }
    if (io->key(kc.S)) { camera.movement.B = true; } // Back
    if (!io->key(kc.S)) { camera.movement.B = false; }
    if (io->key(kc.D)) { camera.movement.R = true; } // Right
    if (!io->key(kc.D)) { camera.movement.R = false; }
    if (io->key(kc.A)) { camera.movement.L = true; } // Left
    if (!io->key(kc.A)) { camera.movement.L = false; }
    if (io->key(kc.Num0)) { camera.pivot.reset(); } // Reset


    // Scroll for ZOOM
    (io->key(kc.LeftShift)) ? camera.setZoom(io->scrollV()) : camera.setFOV(io->scrollV());
    if (io->key(kc.LeftSuper)) { camera.pivot.modRot({io->axisV(), io->axisH(), 0.f}); }
    // Draw meshes
    vo.mesh.monkey->transform.setPosX(-3.f);
    vo.mesh.monkey->draw(*vo.shader.light, camera.getView(), &tex1);
    vo.mesh.monkey->transform.setPosX(0.f);
    vo.mesh.monkey->draw(*vo.shader.light, camera.getView(), &tex2);
    vo.mesh.monkey->transform.setPosX(3.f);
    vo.mesh.monkey->draw(*vo.shader.light, camera.getView(), &tex3);

    // ImGUI
    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::Bullet();
    ImGui::End();
  };

  vo.run();
  return 0;
}
