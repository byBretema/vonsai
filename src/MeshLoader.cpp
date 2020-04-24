#include <Vonsai/MeshLoader.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Utils/Strings.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <unordered_map>


namespace Vonsai {

RenderablePOD getMeshFromOBJ(std::string const &filePath) {
  RenderablePOD out;
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

// std::vector<RenderablePOD> getMeshFromFile(std::string const &filePath) {
RenderablePOD getMeshFromFile(std::string const &filePath) {

  // * CACHE FRIENDLY

  static std::unordered_map<std::string, std::vector<RenderablePOD>> cache;
  auto [KV, needsProcessing] = cache.try_emplace(filePath, std::vector<RenderablePOD>{});
  auto &&out                 = KV->second;
  if (!needsProcessing) { return out[0]; }


  // * GET & VALIDATE THE SCENE

  Assimp::Importer importer;
  // http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a6afb4fee42eca4482674859196cb8685
  importer.SetPropertyFloat("AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE", 80.f);
  // http://assimp.sourceforge.net/lib_html/postprocess_8h.html#aaa612ed3f167a8999405d53fc309594b
  aiScene const *scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    vo_err("[ASSIMP] : {}", importer.GetErrorString());
    out.clear();
    // return out;
    return RenderablePOD{};
  }
  if (scene->HasAnimations()) {
    vo_warn("Animations are not supported yet!"); //
  }
  if (scene->HasLights() or scene->HasCameras() or scene->HasTextures()) {
    vo_warn("Light, Cameras and Embedded-Textures aren't and won't be supported")
  }


  // * PROCESS SCENE DATA

  if (scene->HasMeshes()) {
    auto &&N = scene->mNumMeshes;
    out.reserve(N);

    for (auto i = 0u; i < N; ++i) {

      auto &&mesh = scene->mMeshes[i];
      if (mesh->HasBones()) { vo_warn("Bones are not supported yet!"); }

      auto &&r  = out.emplace_back();
      auto &&NV = mesh->mNumVertices;
      vo_debug("num vert of {} : {}", filePath, NV);

      if (mesh->HasPositions()) {
        r.vertices.reserve(NV);
        for (auto j = 0u; j < NV; ++j) {
          auto &&v = mesh->mVertices[j];
          r.vertices.push_back({v.x, v.y, v.z});
        }
      } else {
        vo_err("Mesh '{}' has no positions!", filePath);
        continue; // Stop the processig of current mesh
      }

      if (mesh->HasFaces()) {
        // Due to 'Renderable' design if this code is not hitted.
        // Renderable(s) generated with this RenderablePOD
        // won't be valids.
        r.indices.reserve(mesh->mNumFaces * 3u);
        for (auto j = 0u; j < mesh->mNumFaces; ++j) {
          auto &&face = mesh->mFaces[j];
          if (face.mNumIndices < 3u) { continue; } // Ignore lines and points
          assert(face.mNumIndices == 3);           // We are applying aiProcess_Triangulate
          for (auto k = 0u; k < face.mNumIndices; ++k) { r.indices.push_back(face.mIndices[k]); }
        }
      } else {
        vo_err("Mesh '{}' has no faces!", filePath);
        continue; // Stop the processig of current mesh
      }

      if (mesh->HasNormals()) {
        r.normals.reserve(NV);
        for (auto j = 0u; j < NV; ++j) {
          auto &&v = mesh->mNormals[j];
          r.normals.push_back({v.x, v.y, v.z});
        }
      } else {
        vo_warn("Mesh '{}' has no normals!", filePath);
      }

      if (mesh->HasTangentsAndBitangents()) {
        // Tangents
        r.tangents.reserve(NV);
        for (auto j = 0u; j < NV; ++j) {
          auto &&v = mesh->mTangents[j];
          r.tangents.push_back({v.x, v.y, v.z});
        }
        // BiTangents
        r.bitangents.reserve(NV);
        for (auto j = 0u; j < NV; ++j) {
          auto &&v = mesh->mBitangents[j];
          r.bitangents.push_back({v.x, v.y, v.z});
        }
      } else {
        // vo_warn("Mesh '{}' has no tangents and bitangents!", filePath);
      }

      auto const defaultTexChannel = 0u;
      if (mesh->HasTextureCoords(defaultTexChannel)) {
        if (mesh->mNumUVComponents[defaultTexChannel] == 2u) {
          r.texCoords.reserve(NV);
          for (auto j = 0u; j < NV; ++j) {
            auto &&v = mesh->mTextureCoords[defaultTexChannel][j];
            r.texCoords.push_back({v.x, v.y});
          }
        } else {
          vo_warn("Only 2D textures are supported");
        }
      }

      bool useTextureChannels = false;
      for (auto j = 0u; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++j) {
        if (j == defaultTexChannel) { continue; }
        useTextureChannels |= mesh->HasTextureCoords(j);
      }
      if (useTextureChannels) { vo_warn("Texture channels aren't and won't be supported.") }

    } // End mesh-processing
  }

  // if (scene->HasMaterials()) {
  //   auto &&N = scene->mNumMaterials;

  //   std::vector<aiMaterial> materials;
  //   materials.reserve(N);

  //   for (auto i = 0u; i < N; ++i) {
  //     auto &&curr = scene->mMaterials[i];
  //     vo_print("mat {} name : {}", i, curr->GetName());
  //   }
  // }


  // * THEN COMPOSE THE HIERARCHY OF MESHES
  // auto &&root = scene->mRootNode;


  // * RETURN THE POPULATED DATA
  // return out;
  return out[0];
}

} // namespace Vonsai
