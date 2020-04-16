#include <Vonsai/Engine.hpp>

#include <Vonsai/Wraps/_gl.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Strings.hpp>

namespace Vonsai {
RenderableData parserObj(std::string const &filePath); // * Quick OBJ loader

Engine::Engine() {
  m_ios.emplace("default", std::make_shared<IO>()); // ! Should be the first action

  // mesh.monkey = std::make_shared<Renderable>(parserObj("assets/models/monkey.obj"));
  // mesh.cube   = std::make_shared<Renderable>(parserObj("assets/models/cube.obj"));

  // Vonsai::ShaderPath lightSP;
  // lightSP.vertex   = "assets/shaders/light/light.vert";
  // lightSP.fragment = "assets/shaders/light/light.frag";
  // shader.light     = std::make_shared<Shader>("light", lightSP);

  // Vonsai::ShaderPath flatSP;
  // flatSP.vertex   = "assets/shaders/flat/flat.vert";
  // flatSP.fragment = "assets/shaders/flat/flat.frag";
  // shader.flat     = std::make_shared<Shader>("light", flatSP);
}

void Engine::run() const {
  std::vector<std::string> toClean;
  toClean.reserve(8 /*MaxWindows*/);

  while (m_ios.size() > 0) {
    for (auto &&[winName, winPtr] : m_ios) {
      if (!winPtr->isValid()) {
        toClean.push_back(winName);
        continue;
      }

      winPtr->update();
      if (winPtr->key(Vonsai::KeyCode.Esc)) { winPtr->close(); }
    }

    std::for_each(begin(toClean), end(toClean), [&ios = m_ios](std::string const &key) { ios.erase(key); });
    toClean.clear();
  }
}

std::shared_ptr<IO> Engine::getWindow(std::string const &a_name) const {
  return (m_ios.count(a_name) > 0) ? m_ios.at(a_name) : nullptr;
}


// * //////////////////////////////////////////////////////////////////////////


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
