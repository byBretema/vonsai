#include <Vonsai/Vonsai.hpp>

// =========================================================================

void drawOne(vo::Renderable &R, vo::Shader const &S, vo::Texture const *T, vo::Camera const &cam, bool active);

// =========================================================================

void sandbox() {
  vo::Context ctx;

  // === OBJECTS SETUP -------------------------------------------------------

  /*

  TODO : implement something like...
  auto &&s = ctx.get(EScene::S0);

  * LIGHT *
  vo::Light         l1({0, 2, 5}, {1, 1, 1});
  vo::Light         l2({0, 2, -5}, {1, 1, 1});
  s.setLights({l1,l2});

  * CAMERA - Implies camera list - *
  vo::Camera camera;
  camera.setZoom(17.5f);
  s.addCamera(camera)

  ! Makes the linkage of UBO in the:
    ? Scene side
    ? Shader side

  */

  // --- LIGHT ---
  vo::Light         l1({0, 2, 5}, {1, 1, 1});
  vo::Light         l2({0, 2, -5}, {1, 1, 1});
  std::vector const lv = {l1, l2};

  vo::UBO lightsUBO;
  lightsUBO.setData("u_numLights", glm::vec4{static_cast<float>(lv.size())});
  lightsUBO.setData("u_lights", lv);
  ctx.linkUBO("lights", lightsUBO.getBindPoint());
  // --- / CAMERA ---

  // --- CAMERA ---
  vo::Camera camera;
  camera.setZoom(17.5f);

  vo::UBO cameraUBO;
  ctx.linkUBO("camera", cameraUBO.getBindPoint());
  // --- / CAMERA ---


  std::vector<vo::Texture> textures;
  textures.reserve(16);
  textures.emplace_back("assets/textures/dac.png");
  textures.emplace_back("assets/textures/Vonsai.png");
  textures.emplace_back("assets/textures/chess.jpg");

  int                      bodyTexID = 6;
  std::vector<vo::Texture> bodyTextures;
  bodyTextures.reserve(16);
  bodyTextures.emplace_back("assets/models/kenney/skins/cyborg.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/criminal.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/skater1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/skater2.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/survivor1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/survivor2.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/zombie1.png");
  bodyTextures.emplace_back("assets/models/kenney/skins/zombie2.png");

  // === UI VARS SETUP -------------------------------------------------------

  std::array<bool, 3> show = {false}; // Set all to false
  show[0]                  = true;    // Show by default de simplest Scene

  int   debugMode   = 0;
  int   shadingOpts = 1;
  float normalSize  = 0.05f;

  bool mainWindow{true};


  // === 3D MULTIMESH SETUP --------------------------------------------------

  std::vector<vo::Renderable> nanosuit;
  for (auto &&mesh : Vonsai::Mesh::import("assets/models/nanosuit/nanosuit.fbx")) { nanosuit.emplace_back(mesh); }

  auto renderNanosuit = [&](vo::Shader const &S) {
    for (auto &&renderable : nanosuit) {
      drawOne(renderable, S, nullptr, camera, show[1]); //
    }
  };


  // === SCENE UPDATE SETUP --------------------------------------------------

  auto &&s = ctx.get(EScene::S0);

  s.setOnUpdateFn([&]() {
    s.setClearColor(0.2, 0.1, 0.2);
    camera.defaultBehaviour(s.getDeltaTime(), ctx.getAspectRatio(), cameraUBO, ctx.getInput());
    drawOne(ctx.get(EMesh::BODY), ctx.get(EShader::LIGHT), &bodyTextures.at(bodyTexID), camera, show[0]);

    // if (shadingOpts == 0) {
    //   ctx.getShader(EShader::DEBUG).setFloat1("u_debug_mode", debugMode);
    //   drawOne(ctx.getMesh(EMesh::BODY), ctx.getShader(EShader::DEBUG), &bodyTextures.at(bodyTexID), camera, show[0]);
    //   renderNanosuit(ctx.getShader(EShader::DEBUG));
    //   drawOne(ctx.getMesh(EMesh::DRAGON), ctx.getShader(EShader::DEBUG), nullptr, camera, show[2]);
    // }

    // if (shadingOpts >= 1) {
    //   drawOne(ctx.getMesh(EMesh::BODY), ctx.getShader(EShader::LIGHT), &bodyTextures.at(bodyTexID), camera, show[0]);
    //   renderNanosuit(ctx.getShader(EShader::LIGHT));
    //   drawOne(ctx.getMesh(EMesh::DRAGON), ctx.getShader(EShader::LIGHT), nullptr, camera, show[2]);
    // }
  });


  // == = SCENE GUI SETUP -----------------------------------------------------

  s.setOnGuiFn([&]() {
    if (mainWindow) {
      ImGui::Begin("Main", &mainWindow);

      ImGui::TextColored({1.f, 0.5f, 1.f, 1.f}, "DATA");
      ImGui::Separator();
      ImGui::Text("FPS: %d", s.getFPS());
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


  // === GAME LOOP INVOCATION ------------------------------------------------

  ctx.run();
}


// ! === MAIN ================================================================

int main() {

  try {
    sandbox();
  } catch (std::exception &e) {
    vo_err("EXCEPTION: {}", e.what()); //
  } catch (const char *msg) {
    vo_err("EXCEPTION: {}", msg); //
  } catch (...) {
    vo_err("EXCEPTION: {}", "No Info"); //
  }
}

// ! === / MAIN ==============================================================


// =========================================================================
// HELPERS
// =========================================================================

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void drawOne(vo::Renderable &R, vo::Shader const &S, vo::Texture const *T, vo::Camera const &cam, bool active) {
  if (!active) { return; }
  Vonsai::BindGuard bgR{R};
  Vonsai::BindGuard bgS{S};
  Vonsai::BindGuard bgT{T};
  if (T) { S.setTexture("u_texture", T->getID()); }

  auto &&[modelView, normalMat] = cam.genModelMatrices(R);
  S.setMat4("u_modelView", modelView);
  S.setMat4("u_normalMat", normalMat);

  R.draw();
};

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
