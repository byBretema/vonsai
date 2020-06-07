#include <Vonsai/Utils/Mesh.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Utils/Strings.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <unordered_map>

namespace Vonsai::Mesh {

// Faces
void aiCopyMeshAttrib(std::vector<unsigned int> &out, aiFace const *in, unsigned int numFaces) {
  out.reserve(numFaces * 3u);
  for (auto i = 0u; i < numFaces; ++i) {
    auto &&face = in[i];
    if (face.mNumIndices < 3u) { continue; } // Ignore lines and points
    if (face.mNumIndices != 3) { vo_err("Corrupted mesh, faces bigger than 3 vertices are not supported"); }
    for (auto j = 0u; j < face.mNumIndices; ++j) { out.emplace_back(face.mIndices[j]); }
  }
}

// Textures
void aiCopyMeshAttrib(std::vector<glm::vec2> &out, aiVector3D const *in, unsigned int numVertices) {
  out.reserve(numVertices);
  for (auto i = 0u; i < numVertices; ++i) {
    auto &&v = in[i];
    out.emplace_back(v.x, v.y);
  }
}

// Close to everyting-else
void aiCopyMeshAttrib(std::vector<glm::vec3> &out, aiVector3D const *in, unsigned int numVertices) {
  out.reserve(numVertices);
  for (auto i = 0u; i < numVertices; ++i) {
    auto &&v = in[i];
    out.emplace_back(v.x, v.y, v.z);
  }
}


RenderablePOD meshProcessing(aiMesh const *mesh) {
  RenderablePOD R;

  if (mesh->HasBones()) { vo_warn("Bones are not supported yet!"); }

  if (mesh->HasFaces()) {
    aiCopyMeshAttrib(R.indices, mesh->mFaces, mesh->mNumFaces);
  } else {
    vo_err("Mesh has no faces!");
    return R;
  }

  if (mesh->HasPositions()) {
    aiCopyMeshAttrib(R.positions, mesh->mVertices, mesh->mNumVertices);
  } else {
    vo_err("Mesh has no positions!");
    return R;
  }

  if (mesh->HasNormals()) {
    aiCopyMeshAttrib(R.normals, mesh->mNormals, mesh->mNumVertices);
  } else {
    vo_err("Mesh has no normals!");
  }

  if (mesh->HasTangentsAndBitangents()) {
    aiCopyMeshAttrib(R.tangents, mesh->mTangents, mesh->mNumVertices);
    aiCopyMeshAttrib(R.bitangents, mesh->mBitangents, mesh->mNumVertices);
  } else {
    vo_warn("Mesh has no tangents and bitangents!");
  }

  // Process the 2D texture channel
  bool foundTexCoord{false};
  for (auto j = 0u; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++j) {
    if (!mesh->HasTextureCoords(j)) { continue; }
    if (foundTexCoord) {
      vo_warn("Texture channels aren't and won't be supported.");
      break; // Just check that use more than one channel
    }
    if (mesh->mNumUVComponents[j] != 2u) {
      vo_warn("Only 2D textures are supported");
      continue; // Maybe other channel has an allowed 2D texture
    }
    foundTexCoord = true;
    aiCopyMeshAttrib(R.texCoords, mesh->mTextureCoords[j], mesh->mNumVertices);
  }

  return R;
}


std::vector<RenderablePOD> import(std::string const &filePath) {

  // * CACHE FRIENDLY
  static auto cache = []() {
    std::unordered_map<std::string, std::vector<RenderablePOD>> ca;
    ca.reserve(256);
    return ca;
  }();
  auto [KV, needsProcessing] = cache.try_emplace(filePath, std::vector<RenderablePOD>{});
  auto &&out                 = KV->second;
  if (!needsProcessing) { return out; }


  // * GET & VALIDATE THE SCENE

  Assimp::Importer importer;
  // http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a6afb4fee42eca4482674859196cb8685
  importer.SetPropertyFloat("AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE", 80.f);
  // http://assimp.sourceforge.net/lib_html/postprocess_8h.html#aaa612ed3f167a8999405d53fc309594b
  aiScene const *scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    vo_err("[ASSIMP] : {}", importer.GetErrorString());
    return std::vector<RenderablePOD>{RenderablePOD{}};
  }
  if (scene->HasAnimations()) {
    vo_warn("Animations are not supported yet!"); //
  }
  if (scene->HasLights() or scene->HasCameras() or scene->HasTextures()) {
    vo_warn("Light, Cameras and Embedded-Textures aren't and won't be supported")
  }


  // * PROCESS SCENE DATA

  if (scene->HasMeshes()) {
    out.reserve(scene->mNumMeshes);
    vo_log("Processing file {}", filePath);
    for (auto i = 0u; i < scene->mNumMeshes; ++i) { out.emplace_back(meshProcessing(scene->mMeshes[i])); }
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

  return out;
}

} // namespace Vonsai::Mesh
