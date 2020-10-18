#include <Vonsai/Vonsai.hpp>

auto guiSpace(int count) {
  for (int i = 0; i < count; ++i) ImGui::Spacing();
}
auto guiDiv(int before = 0, int after = 0) {
  guiSpace(before);
  ImGui::Separator();
  guiSpace(after);
}


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
  s.addCamera(camera);

  ! Makes the linkage of UBO in the:
    ? Scene side   [V]
    ? Shader side  [X]
  */

  // --- LIGHT ---
  vo::Light l1({0, 2, 5}, {1, 1, 1});
  vo::Light l2({0, 2, -5}, {1, 1, 1});
  vo::UBO   lightsUBO = ctx.linkLightUBO("lights", {l1, l2});
  // --- / LIGHT ---

  // --- CAMERA ---
  vo::Camera camera(17.5f);
  vo::UBO    cameraUBO = ctx.linkCameraUBO("camera", camera);
  // --- / CAMERA ---


  std::vector<vo::Texture> textures;
  textures.reserve(16);
  textures.emplace_back(vo_res + "textures/dac.png");
  textures.emplace_back(vo_res + "textures/Vonsai.png");
  textures.emplace_back(vo_res + "textures/chess.jpg");

  [[maybe_unused]] int     bodyTexID = 6;
  std::vector<vo::Texture> bodyTextures;
  bodyTextures.reserve(16);
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/cyborg.png");
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/criminal.png");
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/skater1.png");
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/skater2.png");
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/survivor1.png");
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/survivor2.png");
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/zombie1.png");
  bodyTextures.emplace_back(vo_res + "models/kenney/skins/zombie2.png");

  // === UI VARS SETUP -------------------------------------------------------

  int ui_dbgMode = 0;
  int ui_shade   = 0;
  int ui_mesh    = 0;

  // === SCENE UPDATE SETUP --------------------------------------------------

  auto &&     s = ctx.get(EScene::S0);
  vo::Texture textureGlories{"/Users/cambalamas/Desktop/Glories/GloriesTowerMapping.jpg"};


  s.setOnUpdateFn([&]() {
    s.setClearColor(0.2, 0.2, 0.2);
    camera.defaultBehaviour(s.getDeltaTime(), ctx.getAspectRatio(), cameraUBO, ctx.getInput());
    ctx.get(EMesh::GLORIES).drawAsPBR(ctx.get(EShader::PBR), camera);

    // if (ui_shade == 0) {
    //   ctx.get(EShader::DEBUG).setFloat1("u_debug_mode", ui_dbgMode);
    //   vo_draw(ctx.get(EMesh::GLORIES), ctx.get(EShader::DEBUG), &textureGlories, camera, ui_mesh == 0);
    // vo_draw(ctx.get(EMesh::NANOSUIT), ctx.get(EShader::DEBUG), &bodyTextures.at(bodyTexID), camera, ui_mesh == 1);
    // vo_draw(ctx.get(EMesh::DRAGON), ctx.get(EShader::DEBUG), &bodyTextures.at(bodyTexID), camera, ui_mesh == 2);
    // }
    // if (ui_shade >= 1) {
    //   vo_draw(ctx.get(EMesh::GLORIES), ctx.get(EShader::LIGHT), &textureGlories, camera, ui_mesh == 0);
    // vo_draw(ctx.get(EMesh::NANOSUIT), ctx.get(EShader::LIGHT), &bodyTextures.at(bodyTexID), camera, ui_mesh == 1);
    // vo_draw(ctx.get(EMesh::DRAGON), ctx.get(EShader::LIGHT), &bodyTextures.at(bodyTexID), camera, ui_mesh == 2);
    // }
  });


  // == = SCENE GUI SETUP -----------------------------------------------------

  s.setOnGuiFn([&]() {
    ImGui::Begin("Main");

    ImGui::TextColored({1.f, 0.5f, 1.f, 1.f}, "DATA");
    guiSpace(1);
    ImGui::Text("FPS: %d", s.getFPS());
    guiDiv(1, 1);

    // ImGui::TextColored({1.f, 1.f, 0.5f, 1.f}, "MESH PICKER");
    // guiSpace(1);
    // for (auto i = 0u; i < show.size(); ++i) { ImGui::Checkbox(vo_fmt("Scene {}", i).c_str(), &show[i]); }
    // for (auto i = 0u; i < 3; ++i) { ImGui::RadioButton(vo_fmt("Mesh {}", i).c_str(), &ui_mesh, i); }
    // if (ui_mesh == 0) {
    //   ImGui::TextColored({0.75, 1, 1, 1}, "Select skin:");
    //   ImGui::Combo("#Skin", &bodyTexID, "Cyborg\0Criminal\0Skater1\0Skater2\0Survivor1\0Survivor2\0Zombie1\0Zombie2\0",
    //                8);
    // }
    // guiDiv(1, 1);

    auto shadeCounter = 0u;
    ImGui::TextColored(ImVec4(1.f, 1.f, 0.5f, 1.f), "SHADERS");
    guiSpace(1);
    ImGui::RadioButton("Debug", &ui_shade, shadeCounter++);
    if (ui_shade == 0) {
      ImGui::TextColored({0.75, 1, 1, 1}, "What to Debug?");
      ImGui::Combo("#Debug", &ui_dbgMode, "Flat\0UVs\0Normals\0", 3);
    }
    ImGui::RadioButton("Shade", &ui_shade, shadeCounter++);
    guiDiv(1, 1);

    ImGui::End();
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



// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
