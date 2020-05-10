// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <array>

#include <imgui/imgui.h>

#include <Vonsai/Vonsai.hpp>

// #include <Vonsai/Bindable.hpp>
#include <Vonsai/Utils/Colors.hpp>
#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Utils/Mesh.hpp>
#include <Vonsai/Utils/Timer.hpp>

// =========================================================================


// =========================================================================

auto drawOne(vo::Renderable &R, vo::Shader const &S, vo::Texture const *T, vo::Camera const &cam, bool active) {
  if (!active) { return; }
  Vonsai::BindGuard bgS{S};
  Vonsai::BindGuard bgT{T};
  if (T) { S.setTexture("u_texture", T->getID()); }

  auto &&[modelView, normalMat] = cam.genModelMatrices(R);
  S.setMat4("u_modelView", modelView);
  S.setMat4("u_normalMat", normalMat);
  R.draw();

  if (T) { S.setTexture("u_texture", 0); }
};

// =========================================================================

struct {
  std::unique_ptr<vo::Shader> light;
  std::unique_ptr<vo::Shader> normals;
  std::unique_ptr<vo::Shader> debug;
} SHADERS;

struct {
  std::unique_ptr<vo::Renderable> monkey;
  std::unique_ptr<vo::Renderable> cube;
  std::unique_ptr<vo::Renderable> plane;
  std::unique_ptr<vo::Renderable> chandelier;
  std::unique_ptr<vo::Renderable> body;
  std::unique_ptr<vo::Renderable> dragon;
} MESHES;

void initDefaults() {

  MESHES.monkey     = std::make_unique<vo::Renderable>(Vonsai::Mesh::import("assets/models/monkey.obj").at(0));
  MESHES.cube       = std::make_unique<vo::Renderable>(Vonsai::Mesh::import("assets/models/cube.fbx").at(0));
  MESHES.plane      = std::make_unique<vo::Renderable>(Vonsai::Mesh::import("assets/models/plane.obj").at(0));
  MESHES.chandelier = std::make_unique<vo::Renderable>(Vonsai::Mesh::import("assets/models/chandelier.obj").at(0));

  MESHES.body = std::make_unique<vo::Renderable>(Vonsai::Mesh::import("assets/models/kenney/character.fbx").at(0));
  MESHES.body->transform.modRotX(-90.f);
  MESHES.body->transform.modPosY(-1.75f);

  MESHES.dragon = std::make_unique<vo::Renderable>(Vonsai::Mesh::import("assets/models/dragon.obj").at(0));

  Vonsai::ShaderPath lightSP;
  lightSP.vertex   = "assets/shaders/light/light.vert";
  lightSP.geometry = "assets/shaders/light/light.geom";
  lightSP.fragment = "assets/shaders/light/light.frag";
  SHADERS.light    = std::make_unique<vo::Shader>("light", lightSP);

  Vonsai::ShaderPath normalsSP;
  normalsSP.vertex   = "assets/shaders/normals/normals.vert";
  normalsSP.geometry = "assets/shaders/normals/normals.geom";
  normalsSP.fragment = "assets/shaders/normals/normals.frag";
  SHADERS.normals    = std::make_unique<vo::Shader>("normals", normalsSP);

  Vonsai::ShaderPath debugSP;
  debugSP.vertex   = "assets/shaders/debug/debug.vert";
  debugSP.geometry = "assets/shaders/debug/debug.geom";
  debugSP.fragment = "assets/shaders/debug/debug.frag";
  SHADERS.debug    = std::make_unique<vo::Shader>("debug", debugSP);
}

// =========================================================================

void sandbox() {
  vo::Scene*  scene;
  vo::Scene  scene1{};
  vo::Scene  scene2{};

  vo::Input  input{};
  vo::Window window{&input, 800, 600};
  initDefaults(); // ! AFTER WINDOW INIT

  // === OBJECTS SETUP ========================================================

  vo::Light              l1({0, 2, 5}, {1, 1, 1});
  vo::Light              l2({0, 2, -5}, {1, 1, 1});
  std::vector<vo::Light> lv{l1, l2};

  vo::UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  SHADERS.light->linkUBO("lights", lightsUBO.getBindPoint());
  SHADERS.debug->linkUBO("lights", lightsUBO.getBindPoint());
  // SHADERS.normals->linkUBO("lights", lightsUBO.getBindPoint());

  vo::UBO cameraUBO;
  SHADERS.light->linkUBO("camera", cameraUBO.getBindPoint()); // TODO : make this transparent
  SHADERS.debug->linkUBO("camera", cameraUBO.getBindPoint());
  SHADERS.normals->linkUBO("camera", cameraUBO.getBindPoint());

  std::vector<vo::Texture> textures{};
  textures.emplace_back("assets/textures/dac.png");
  textures.emplace_back("assets/textures/Vonsai.png");
  textures.emplace_back("assets/textures/chess.jpg");

  int                      bodyTexID = 6;
  std::vector<vo::Texture> bodyTextures;
  bodyTextures.emplace_back("assets/models/kenney/skins/cyborg.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/criminal.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/skater1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/skater2.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/survivor1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/survivor2.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/zombie1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/zombie2.png");

  vo::Camera camera;
  camera.setZoom(17.5f);

  // === / OBJECTS SETUP ======================================================


  // === UI VARS SETUP ========================================================

  std::array<bool, 3> show = {false}; // Set all to false
  show[0]                  = true;    // Show by default de simplest Scene

  int   debugMode   = 0;
  int   shadingOpts = 1;
  float normalSize  = 0.05f;

  bool mainWindow{true};

  // === / UI VARS SETUP ======================================================


  // === MODELS STUFF SETUP ===================================================

  std::vector<vo::Renderable> nanosuit;
  for (auto &&mesh : Vonsai::Mesh::import("assets/models/nanosuit/nanosuit.fbx")) { nanosuit.emplace_back(mesh); }

  auto renderNanosuit = [&](vo::Shader const &S) {
    for (auto &&renderable : nanosuit) {
      drawOne(renderable, S, nullptr, camera, show[1]); //
    }
  };

  // === / MODELS STUFF SETUP =================================================


  // === SCENE UPDATE SETUP ===================================================

  scene1.setOnUpdateFn([&, &input = input, &window = window]() {
    camera.defaultBehaviour(scene1.getDeltaTime(), window.getAspectRatio(), cameraUBO, input);

    if (shadingOpts == 0) {
      SHADERS.debug->setFloat1("u_debug_mode", debugMode);
      drawOne(*MESHES.body, *SHADERS.debug, &bodyTextures.at(bodyTexID), camera, show[0]);
      renderNanosuit(*SHADERS.debug);
      drawOne(*MESHES.dragon, *SHADERS.debug, nullptr, camera, show[2]);
    }

    if (shadingOpts >= 1) {
      drawOne(*MESHES.body, *SHADERS.light, &bodyTextures.at(bodyTexID), camera, show[0]);
      renderNanosuit(*SHADERS.light);
      drawOne(*MESHES.dragon, *SHADERS.light, nullptr, camera, show[2]);
    }
  });
  // === / SCENE UPDATE SETUP =================================================

  // === SCENE GUI SETUP ======================================================

  scene1.setOnGuiFn([&]() {
    if (mainWindow) {
      ImGui::Begin("Main", &mainWindow);

      ImGui::TextColored({1.f, 0.5f, 1.f, 1.f}, "DATA");
      ImGui::Separator();
      ImGui::Text("FPS: %d", scene1.getFPS());
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

  // === / SCENE GUI SETUP ====================================================


  // === GAME LOOP ============================================================

  unsigned int currScene = 0u;
  std::vector scenes = {scene1, scene2};

  if(scenes.size() < 1) { return; }
  scene = &scenes[0];

  while (window.update(scene->getOnUpdateFn(), scene->getOnGuiFn())) {
    input.resetScrollAndAxis();
    if (input.key(vo::KeyCode::Esc)) { window.close(); }
    if (input.key(vo::KeyCode::Right)) { scene = &scenes.at(++currScene % scenes.size()); }
    if (input.key(vo::KeyCode::Left)) { scene = &scenes.at(--currScene % scenes.size()); }
  }
  window.shutdown();

  // === / GAME LOOP ==========================================================
}

int main() {
  try {
    sandbox();
  } catch (std::exception &e) {
    vo_err("EXCEPTION: {}", e.what()); //
  } catch (const char *msg) {
    vo_err("EXCEPTION: {}", msg); //
  } catch (...) {
    vo_err("EXCEPTION: {}", "No Info"); //
    throw;                              // Re-throw the exception so OS gives you a debug opportunity.
  }
}
