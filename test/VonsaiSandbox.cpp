// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <array>

#include <imgui/imgui.h>

#include <Vonsai/Bindable.hpp>
#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Vonsai.hpp>

#include <Vonsai/MeshLoader.hpp>

using Light  = vo::Light;
using UBO    = vo::UBO;
using Camera = vo::Camera;

using Texture    = vo::Texture;
using Shader     = vo::Shader;
using Renderable = vo::Renderable;

namespace KC = vo::KeyCode;


int main() {

  Vonsai::Engine voe;
  auto &&[input, window] = voe.getIO();
  auto &&sc              = voe.getActiveScene();

  // ========================================================================================== //

  Light              l1({0, 2, 5}, {1, 1, 1});
  Light              l2({0, 2, -5}, {1, 1, 1});
  std::vector<Light> lv{l1, l2};

  UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  voe.shader.light->linkUBO("lights", lightsUBO.getBindPoint());

  UBO cameraUBO;
  voe.shader.light->linkUBO("camera", cameraUBO.getBindPoint());
  voe.shader.normals->linkUBO("camera", cameraUBO.getBindPoint());

  std::vector<Texture> textures{};
  textures.emplace_back("assets/textures/dac.png");
  textures.emplace_back("assets/textures/Vonsai.png");
  textures.emplace_back("assets/textures/chess.jpg");

  Camera camera;
  // camera.pivot.modPosX(+3.f);
  camera.setZoom(17.5f);

  // ========================================================================================== //



  // sc.setClearColor(0.6, 0.3, 0.1);

  auto drawOne = [&](Renderable &R, Shader const &S, Texture *T, bool active) {
    if (!active) { return; }
    Vonsai::BindGuard bgS{S};
    if (T) {
      // Vonsai::BindGuard bgT{T}; // ! Fails if Texture*
      T->bind();
      S.setTexture("u_texture", T->getID());
    }

    auto const modelView = camera.getView() * R.transform.matrix();
    S.setMat4("u_modelView", modelView);
    // S.setMat4("u_normalMat", glm::transpose(glm::inverse(modelView)));

    R.draw();
    if (T) {
      T->unbind();
      S.setTexture("u_texture", 0);
    }
  };


  Renderable r1{Vonsai::getMeshFromOBJ("assets/models/monkey.obj")};
  Renderable r2{Vonsai::getMeshFromFile("assets/models/monkey.obj")};

  std::array<bool, 2> show = {false}; // Set all to false
  show[0]                  = true;    // Show by default de simplest Scene

  int   shadingOpts = 0;
  float normalSize  = 0.05f;

  sc.setOnUpdateFn([&, &input = input, &window = window]() {
    camera.defaultBehaviour(sc.getDeltaTime(), window.getAspectRatio(), cameraUBO, input);

    if (shadingOpts >= 0) {
      drawOne(r1, *voe.shader.light, &textures[1], show[0]);
      drawOne(r2, *voe.shader.light, &textures[1], show[1]);
    }

    if (shadingOpts == 1) {
      voe.shader.normals->setFloat1("u_normalSize", normalSize);
      voe.shader.normals->setFloat3("u_normalColor", {1, 1, 0});
      drawOne(r1, *voe.shader.normals, nullptr, show[0]);
      voe.shader.normals->setFloat3("u_normalColor", {1, 0, 1});
      drawOne(r2, *voe.shader.normals, nullptr, show[1]);
    }

    // draw(*voe.mesh.monkey, *voe.shader.light, textures[1], 3u, show[0]);
    // draw(*voe.mesh.monkey, *voe.shader.light, textures[0], 5'000u, show[1]);
    // draw(*voe.mesh.chandelier, *voe.shader.light, textures[2], 275u, show[2]);
  });

  bool mainWindow{true};
  sc.setOnGuiFn([&]() {
    if (mainWindow) {
      ImGui::Begin("Main", &mainWindow);

      ImGui::TextColored({1.f, 0.5f, 1.f, 1.f}, "DATA");
      ImGui::Separator();
      ImGui::Text("FPS: %d", sc.getFPS());
      ImGui::Separator();

      ImGui::Spacing();

      ImGui::TextColored({1.f, 1.f, 0.5f, 1.f}, "SCENES");
      ImGui::Separator();
      for (auto i = 0u; i < show.size(); ++i) { ImGui::Checkbox(vo_fmt("Scene {}", i).c_str(), &show[i]); }
      ImGui::Separator();

      ImGui::Spacing();

      ImGui::TextColored(ImVec4(1.f, 1.f, 0.5f, 1.f), "SHADERS");
      ImGui::Separator();
      auto shadingOptsCounter = 0u;
      ImGui::RadioButton("Shade", &shadingOpts, shadingOptsCounter++);
      ImGui::RadioButton("Shade and Normals", &shadingOpts, shadingOptsCounter++);
      static auto normalSizeInitValue = normalSize;
      if (shadingOpts == 1) {
        ImGui::TextColored({0.75, 1, 1, 1}, "Normal size:");
        ImGui::SliderFloat("s", &normalSize, normalSizeInitValue, normalSizeInitValue + 0.75f);
      }
      ImGui::Separator();

      ImGui::End();
    }
  });

  voe.run();
  return 0;
}
