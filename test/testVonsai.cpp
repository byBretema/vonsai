// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include <Vonsai/Camera.hpp>
#include <Vonsai/Renderable.hpp>
#include <Vonsai/Shader.hpp>
#include <Vonsai/Texture.hpp>
#include <Vonsai/UBO.hpp>
#include <Vonsai/Window.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Utils/Strings.hpp>

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <thread>
#include <vector>

#include <sstream>

Vonsai::RenderableData parserObj(std::string const &filePath) {
  Vonsai::RenderableData out;
  if (!Vonsai::Files::isValid(filePath, true)) { return out; }

  std::once_flag resizeOnce;
  struct {
    bool texCoords;
    bool normals;
  } exists;

  std::string            linestream;
  std::vector<glm::vec2> uvsTemp;
  std::vector<glm::vec3> normsTemp;
  std::stringstream      filestream(Vonsai::Files::fromString(filePath));

  // Parse begin ...
  while (std::getline(filestream, linestream)) {
    auto line  = Vonsai::Strings::split(linestream, " "); // 1
    auto token = line.at(0);                              // 2

    if ((token == "#") or (token == "s")) { continue; }
    if (token == "v") {
      assert(line.size() == 4);
      out.vertices.push_back({std::stof(line.at(1)), std::stof(line.at(2)), std::stof(line.at(3))});
    }
    if (token == "vt") {
      assert(line.size() == 3);
      uvsTemp.push_back({std::stof(line.at(1)), std::stof(line.at(2))});
    }
    if (token == "vn") {
      assert(line.size() == 4);
      normsTemp.push_back({std::stof(line.at(1)), std::stof(line.at(2)), std::stof(line.at(3))});
    }
    if (token == "f") {
      std::call_once(resizeOnce, [&]() {
        exists.normals   = (normsTemp.size() > 0);
        exists.texCoords = (uvsTemp.size() > 0);
        if (exists.normals) { out.normals.resize(out.vertices.size()); }
        if (exists.texCoords) { out.texCoords.resize(out.vertices.size()); }
      });

      // Process vertex, normals and UVs indices
      for (auto i = 1u; i < line.size(); ++i) {
        std::vector<std::string> face;
        unsigned int             iV, iT, iN;

        if (exists.texCoords and exists.normals) {
          face = Vonsai::Strings::split(line.at(i), "/");
          iT   = std::stoul(face.at(1)) - 1u;
          iN   = std::stoul(face.at(2)) - 1u;
        } else if (!exists.texCoords and exists.normals) {
          face = Vonsai::Strings::split(line.at(i), "//");
          iN   = std::stoul(face.at(1)) - 1u;
        } else if (exists.texCoords and !exists.normals) {
          face = Vonsai::Strings::split(line.at(i), "/");
          iT   = std::stoul(face.at(1)) - 1u;
        } else {
          vo_err("Undefined Normals and UVs @ {}", filePath);
        }
        iV = std::stoul(face.at(0)) - 1u;

        // Fit on vertex index the assigned norm and texcoord
        if (exists.texCoords) out.texCoords[iV] = uvsTemp.at(iT);
        if (exists.normals) out.normals[iV] = normsTemp.at(iN);

        // Store vertex index
        out.indices.push_back(iV);
      }
    }
  }
  return out;
}


int main() {
  // Vonsai::App app;


  // Vonsai::Window w1(320, 240, "window 1", [](Vonsai::Window &_window, int _modifier) {
  //   if (_window.getKey(GLFW_KEY_0)) { _window.setClearColor({0, 1, 0}); }
  //   if (_window.getKey(GLFW_KEY_1)) { std::cout << "1\n"; }
  // });

  // Vonsai::Window w2(320, 240, "window 2", [](Vonsai::Window &_window, int _modifier) {
  //   if (_window.getKey(GLFW_KEY_0)) { _window.setClearColor({1, 1, 0}); }
  //   if (_window.getKey(GLFW_KEY_1)) { std::cout << "2\n"; }
  // });

  // Vonsai::Window w3(320, 240, "window 3", [](Vonsai::Window &_window, int _modifier) {
  //   if (_window.getKey(GLFW_KEY_0)) { _window.setClearColor({1, 1, 1}); }
  //   if (_window.getKey(GLFW_KEY_1)) { std::cout << "3\n"; }
  // });

  // Vonsai::Window::run();


  // Vonsai::App app;
  // app.init();

  // ========================================================================================== //
  Vonsai::Window w1(800, 600, "Vonsai :: Window 1");


  Vonsai::ShaderPath sp;
  sp.vertex   = "assets/shaders/base/base.vert";
  sp.fragment = "assets/shaders/base/base.frag";

  Vonsai::Shader sBase("base", sp);
  Vonsai::UBO    globalData = Vonsai::UBO();
  sBase.setUniformBlock("globalData", globalData.getBindPoint());

  sBase.bind();

  // ========================================================================================== //

  Vonsai::RenderableData obj1 = parserObj("assets/models/monkey.obj");

  Vonsai::Renderable r1 = Vonsai::Renderable(obj1);
  Vonsai::Renderable r2 = Vonsai::Renderable(obj1);
  Vonsai::Renderable r3 = Vonsai::Renderable(obj1);

  r2.transform.pos += glm::vec3{3.f, 0.f, 0.f};
  r3.transform.pos += glm::vec3{6.f, 0.f, 0.f};

  // ========================================================================================== //

  glDisable(GL_CULL_FACE);
  float mokeysDist = (r1.transform.pos.x + r3.transform.pos.x);
  w1.camera.pivot.pos += glm::vec3{mokeysDist / 2.f, 0.f, (mokeysDist + 2.f) * 2.f};

  // ========================================================================================== //

  Vonsai::Texture tex1 = Vonsai::Texture("assets/textures/Vonsai.png");
  tex1.bind();
  Vonsai::Texture tex2 = Vonsai::Texture("assets/textures/dac.png");
  tex2.bind();
  Vonsai::Texture tex3 = Vonsai::Texture("assets/textures/chess.jpg");
  tex3.bind();

  // ========================================================================================== //

  w1.setRenderFn([&]() {
    w1.camera.frame(w1.getAspectRatio());

    globalData.setData("u_view", w1.camera.getView());
    globalData.setData("u_proj", w1.camera.getProj());

    if (!sBase.isReady()) { return; }

    r1.draw();
    sBase.setUniformInt1("u_texture", tex1.getID());
    sBase.setUniformMat4("u_model", r1.transform.matrix());
    r2.draw();
    sBase.setUniformInt1("u_texture", tex2.getID());
    sBase.setUniformMat4("u_model", r2.transform.matrix());
    r3.draw();
    sBase.setUniformInt1("u_texture", tex3.getID());
    sBase.setUniformMat4("u_model", r3.transform.matrix());
  });

  Vonsai::Window::run();
}
