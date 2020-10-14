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
  for (auto i = 0u; i < numFaces; ++i)
    for (auto j = 0u; (in[i].mNumIndices == 3 and j < in[i].mNumIndices); ++j) { out.emplace_back(in[i].mIndices[j]); }
}
// Textures
void aiCopyMeshAttrib(std::vector<glm::vec2> &out, aiVector3D const *in, unsigned int numVertices) {
  out.reserve(numVertices);
  for (auto i = 0u; i < numVertices; ++i) { out.emplace_back(in[i].x, in[i].y); }
}
// Close to everyting-else
void aiCopyMeshAttrib(std::vector<glm::vec3> &out, aiVector3D const *in, unsigned int numVertices) {
  out.reserve(numVertices);
  for (auto i = 0u; i < numVertices; ++i) { out.emplace_back(in[i].x, in[i].y, in[i].z); }
}


RenderablePOD meshProcessing(aiMesh const *mesh) {
  RenderablePOD R;
  aiCopyMeshAttrib(R.indices, mesh->mFaces, mesh->mNumFaces);
  aiCopyMeshAttrib(R.positions, mesh->mVertices, mesh->mNumVertices);
  if (mesh->HasNormals()) { aiCopyMeshAttrib(R.normals, mesh->mNormals, mesh->mNumVertices); }
  if (mesh->HasTangentsAndBitangents()) {
    aiCopyMeshAttrib(R.tangents, mesh->mTangents, mesh->mNumVertices);
    aiCopyMeshAttrib(R.bitangents, mesh->mBitangents, mesh->mNumVertices);
  }
  for (auto j = 0u; (!R.texCoords.size() and j < AI_MAX_NUMBER_OF_TEXTURECOORDS); ++j) {
    if (!mesh->HasTextureCoords(j)) { continue; }
    aiCopyMeshAttrib(R.texCoords, mesh->mTextureCoords[j], mesh->mNumVertices);
  }
  return R;
}


static auto cache = []() {
  std::unordered_map<std::string, std::vector<RenderablePOD>> ca;
  ca.reserve(256);
  return ca;
}();

static constexpr auto AssimpTexTypes = {aiTextureType_DIFFUSE,   aiTextureType_SPECULAR,   aiTextureType_AMBIENT,
                                        aiTextureType_EMISSIVE,  aiTextureType_HEIGHT,     aiTextureType_NORMALS,
                                        aiTextureType_SHININESS, aiTextureType_OPACITY,    aiTextureType_DISPLACEMENT,
                                        aiTextureType_LIGHTMAP,  aiTextureType_REFLECTION, aiTextureType_UNKNOWN};

// http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a6afb4fee42eca4482674859196cb8685
// http://assimp.sourceforge.net/lib_html/postprocess_8h.html#aaa612ed3f167a8999405d53fc309594b
std::vector<RenderablePOD> import(std::string const &filePath) {

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::

  auto [KV, needsProcessing] = cache.try_emplace(filePath, std::vector<RenderablePOD>{});
  auto &&out                 = KV->second;
  if (!needsProcessing) { return out; }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::

  Assimp::Importer importer;
  importer.SetPropertyFloat("AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE", 80.f);
  aiScene const *scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

  if (!scene or scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE or !scene->mRootNode) {
    vo_err("[ASSIMP] : {}", importer.GetErrorString());
    return out;
  }
  // if (scene->HasAnimations() or scene->HasLights() or scene->HasCameras() or scene->HasTextures()) {
  //   vo_warn("Animations, Lights, Cameras and Embedded-Textures aren't supported yet.");
  // }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::

  if (scene->HasMeshes()) {
    // vo_log("Processing file '{}'", filePath);
    out.reserve(scene->mNumMeshes);
    for (auto i = 0u; i < scene->mNumMeshes; ++i) { out.emplace_back(meshProcessing(scene->mMeshes[i])); }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::

  if (scene->HasMaterials()) {
    std::vector<aiMaterial> materials;
    materials.reserve(scene->mNumMaterials);
    for (auto i = 0u; i < scene->mNumMaterials; ++i) {
      auto &&mat = scene->mMaterials[i];
      vo_print("mat {} name : {}", i, mat->GetName().C_Str());

      //--- TEXTURES
      for (auto texType : AssimpTexTypes) {

        if (int totalTexOfCurrType = mat->GetTextureCount(texType); totalTexOfCurrType > 0) {
          aiString texturePath;
          for (auto j = 0; j < totalTexOfCurrType; ++j) {
            mat->GetTexture(texType, j, &texturePath);
            vo_log("texturePath '{}'", texturePath.C_Str());
          }
        }
      }
      //-/- TEXTURES

      //--- RAW PROPS
      // ...
      //-/- RAW PROPS
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::

  // THEN COMPOSE THE HIERARCHY OF MESHES
  // auto &&root = scene->mRootNode;

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::

  return out;
}

} // namespace Vonsai::Mesh
