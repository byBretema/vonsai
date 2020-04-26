// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <array>

#include <imgui/imgui.h>

#include <Vonsai/Bindable.hpp>
#include <Vonsai/Utils/Colors.hpp>
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


auto drawOne = [](Renderable &R, Shader const &S, Texture *T, Camera const &cam, bool active) {
  if (!active) { return; }
  Vonsai::BindGuard bgS{S};
  if (T) {
    // Vonsai::BindGuard bgT{T}; // ! Fails if Texture*
    T->bind();
    S.setTexture("u_texture", T->getID());
  }

  auto &&[modelView, normalMat] = cam.genModelMatrices(R);
  S.setMat4("u_modelView", modelView);
  S.setMat4("u_normalMat", normalMat);
  R.draw();

  if (T) {
    T->unbind();
    S.setTexture("u_texture", 0);
  }
};


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
  voe.shader.debug->linkUBO("lights", lightsUBO.getBindPoint());
  // voe.shader.normals->linkUBO("lights", lightsUBO.getBindPoint());

  UBO cameraUBO;
  voe.shader.light->linkUBO("camera", cameraUBO.getBindPoint()); // TODO : make this transparent
  voe.shader.debug->linkUBO("camera", cameraUBO.getBindPoint());
  voe.shader.normals->linkUBO("camera", cameraUBO.getBindPoint());

  std::vector<Texture> textures{};
  textures.emplace_back("assets/textures/dac.png");
  textures.emplace_back("assets/textures/Vonsai.png");
  textures.emplace_back("assets/textures/chess.jpg");

  int                  bodyTexID = 6;
  std::vector<Texture> bodyTextures;
  bodyTextures.emplace_back("assets/models/kenney/skins/cyborg.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/criminal.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/skater1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/skater2.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/survivor1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/survivor2.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/zombie1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/zombie2.png");

  Camera camera;
  camera.setZoom(17.5f);

  // ========================================================================================== //


  std::array<bool, 3> show = {false}; // Set all to false
  show[0]                  = true;    // Show by default de simplest Scene

  int   debugMode   = 0;
  int   shadingOpts = 1;
  float normalSize  = 0.05f;

  std::vector<Renderable> nanosuit;
  for (auto &&mesh : Vonsai::getMeshFromFile("assets/models/nanosuit/nanosuit.fbx")) { nanosuit.emplace_back(mesh); }

  auto renderNanosuit = [&](Shader const &S) {
    for (auto &&renderable : nanosuit) {
      drawOne(renderable, S, nullptr, camera, show[1]); //
    }
  };

  sc.setOnUpdateFn([&, &input = input, &window = window]() {
    camera.defaultBehaviour(sc.getDeltaTime(), window.getAspectRatio(), cameraUBO, input);

    if (shadingOpts == 0) {
      voe.shader.debug->setFloat1("u_debug_mode", debugMode);
      drawOne(*voe.mesh.body, *voe.shader.debug, &bodyTextures.at(bodyTexID), camera, show[0]);
      renderNanosuit(*voe.shader.debug);
      // drawOne(*voe.mesh.nanosuit, *voe.shader.debug, nullptr, camera, show[1]);
      drawOne(*voe.mesh.cube, *voe.shader.debug, nullptr, camera, show[2]);
    }

    if (shadingOpts >= 1) {
      drawOne(*voe.mesh.body, *voe.shader.light, &bodyTextures.at(bodyTexID), camera, show[0]);
      renderNanosuit(*voe.shader.light);
      drawOne(*voe.mesh.cube, *voe.shader.light, nullptr, camera, show[2]);
    }

    // if (shadingOpts == 2) {
    //   voe.shader.normals->setFloat1("u_normalSize", normalSize);

    //   static auto &&rColor1 = Vonsai::Colors::random();
    //   voe.shader.normals->setFloat3("u_normalColor", rColor1);
    //   drawOne(*voe.mesh.body, *voe.shader.normals, &bodyTextures.at(bodyTexID), camera, show[0]);

    //   static auto &&rColor2 = Vonsai::Colors::random();
    //   voe.shader.normals->setFloat3("u_normalColor", rColor2);
    //   drawOne(*voe.mesh.nanosuit, *voe.shader.normals, nullptr, camera, show[1]);

    //   static auto &&rColor3 = Vonsai::Colors::random();
    //   voe.shader.normals->setFloat3("u_normalColor", rColor3);
    //   drawOne(*voe.mesh.cube, *voe.shader.normals, nullptr, camera, show[1]);
    // }

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
      if (show[0]) {
        ImGui::TextColored({0.75, 1, 1, 1}, "Select skin:");
        ImGui::Combo("", &bodyTexID, "Cyborg\0Criminal\0Skater1\0Skater2\0Survivor1\0Survivor2\0Zombie1\0Zombie2\0", 8);
      }
      ImGui::Separator();

      ImGui::Spacing();

      ImGui::TextColored(ImVec4(1.f, 1.f, 0.5f, 1.f), "SHADERS");
      ImGui::Separator();

      auto shadingOptsCounter = 0u;
      ImGui::RadioButton("Debug", &shadingOpts, shadingOptsCounter++);
      if (shadingOpts == 0) {
        ImGui::TextColored({0.75, 1, 1, 1}, "What to Debug?");
        ImGui::Combo("", &debugMode, "Flat\0UVs\0Normals\0", 3);
      }
      ImGui::RadioButton("Shade", &shadingOpts, shadingOptsCounter++);
      // ImGui::RadioButton("Shade and Normals", &shadingOpts, shadingOptsCounter++);
      static auto normalSizeInitValue = normalSize;
      if (shadingOpts == 2) {
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
