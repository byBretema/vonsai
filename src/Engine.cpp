#include <Vonsai/Engine.hpp>

#include <Vonsai/Wraps/_gl.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Strings.hpp>

namespace Vonsai {
RenderableData parserObj(std::string const &filePath); // * Quick OBJ loader

Engine::Engine() {
  m_ios.emplace_back(std::make_unique<IO>());

  mesh.monkey = std::make_unique<Renderable>(parserObj("assets/models/monkey.obj"));
  mesh.cube   = std::make_unique<Renderable>(parserObj("assets/models/cube.obj"));

  Vonsai::ShaderPath lightSP;
  lightSP.vertex   = "assets/shaders/light/light.vert";
  lightSP.fragment = "assets/shaders/light/light.frag";
  shader.light     = std::make_unique<Shader>("light", lightSP);

  Vonsai::ShaderPath flatSP;
  flatSP.vertex   = "assets/shaders/flat/flat.vert";
  flatSP.fragment = "assets/shaders/flat/flat.frag";
  shader.flat     = std::make_unique<Shader>("light", flatSP);
}

void Engine::run() {
  while (m_ios.size() > 0) {
    for (auto &&io : m_ios) {
      io->update();
      if (io->key(Vonsai::KeyCode.Esc)) { io->close(); }
    }
    m_ios.remove_if([](auto &&io) { return !io->isValid(); });
  }
}



// void Engine::addWindow(std::string const &a_name, int a_width, int a_height) {
//   m_ioVec.emplace_back();
//   m_ioTable.emplace(a_name, m_ioVec.size() - 1);
// }
// void Engine::addScene(std::string const &a_windowName, std::string const &a_sceneName, std::string const &a_hexColor) {
//   // TODO !!
//   return;
// }

// IO App::newWindow(int a_width, int a_height, std::string const &a_name) {
//   return Window(a_width, a_height, a_name, a_inputFn);
// }

RenderableData parserObj(std::string const &filePath) {
  RenderableData out;
  if (!Files::isValid(filePath, true)) { return out; }

  std::once_flag resizeOnce;
  struct {
    bool texCoords;
    bool normals;
  } exists;

  std::string            linestream;
  std::vector<glm::vec2> uvsTemp;
  std::vector<glm::vec3> normsTemp;
  std::stringstream      filestream(Files::fromString(filePath));

  // Parse begin ...
  while (std::getline(filestream, linestream)) {
    auto line  = Strings::split(linestream, " "); // 1
    auto token = line.at(0);                      // 2

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
          face = Strings::split(line.at(i), "/");
          iT   = std::stoul(face.at(1)) - 1u;
          iN   = std::stoul(face.at(2)) - 1u;
        } else if (!exists.texCoords and exists.normals) {
          face = Strings::split(line.at(i), "//");
          iN   = std::stoul(face.at(1)) - 1u;
        } else if (exists.texCoords and !exists.normals) {
          face = Strings::split(line.at(i), "/");
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


} // namespace Vonsai
