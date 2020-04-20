// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <array>

#include <imgui/imgui.h>

#include <Vonsai/Bindable.hpp>
#include <Vonsai/Vonsai.hpp>

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
  voe.shader.light->setUniformBlock("lights", lightsUBO.getBindPoint());

  UBO cameraUBO;
  voe.shader.light->setUniformBlock("camera", cameraUBO.getBindPoint());

  Texture tex1("assets/textures/dac.png");
  Texture tex2("assets/textures/Vonsai.png");
  Texture tex3("assets/textures/chess.jpg");

  Camera camera;
  camera.setZoom(17.5f);

  // ========================================================================================== //



  // sc.setClearColor(0.6, 0.3, 0.1);


  auto draw = [&](Renderable &R, Shader const &S, Texture const &T, unsigned int size, bool active) {
    if (!active) { return; }

    Vonsai::BindGuard bgS{S};
    Vonsai::BindGuard bgT{T};
    S.setUniformInt1("u_texture", T.getID());

    float pos = 0.f;
    for (auto i = 0u; i < size; ++i) {
      R.transform.setPosX(pos);
      pos += 3.f;

      auto const modelView = camera.getView() * R.transform.matrix();
      S.setUniformMat4("u_modelView", modelView);
      S.setUniformMat4("u_normalMat", glm::transpose(glm::inverse(modelView)));

      R.draw();
    }
    S.setUniformInt1("u_texture", 0);
  };


  std::array<bool, 2> show{false, false};
  sc.setOnUpdateFn([&, &input = input, &window = window]() {
    camera.defaultBehaviour(sc.getDeltaTime(), window.getAspectRatio(), cameraUBO, input);
    draw(*voe.mesh.monkey, *voe.shader.light, tex1, 5'000u, show[0]);
    draw(*voe.mesh.chandelier, *voe.shader.light, tex2, 275u, show[1]);
  });

  bool showToggle{true};
  sc.setOnGuiFn([&]() {
    if (showToggle) {
      ImGui::Begin("* TOGGLES", &showToggle);
      ImGui::Checkbox("Draw 1", &show[0]);
      ImGui::Checkbox("Draw 2", &show[1]);
      ImGui::End();
    }
  });

  voe.run();
  return 0;
}
