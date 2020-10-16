#include <Vonsai/Utils/Mesh.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Utils/Strings.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <unordered_map>

namespace Vonsai::Mesh {

//---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<aiTextureType, VoTexs> VoAssimpTex = {{aiTextureType_DIFFUSE, VoTexs::DIFFUSE},
                                                               {aiTextureType_SPECULAR, VoTexs::SPECULAR},
                                                               {aiTextureType_AMBIENT, VoTexs::AMBIENT},
                                                               {aiTextureType_EMISSIVE, VoTexs::EMISSIVE},
                                                               {aiTextureType_HEIGHT, VoTexs::HEIGHT},
                                                               {aiTextureType_NORMALS, VoTexs::NORMALS},
                                                               {aiTextureType_SHININESS, VoTexs::SHININESS},
                                                               {aiTextureType_OPACITY, VoTexs::OPACITY},
                                                               {aiTextureType_DISPLACEMENT, VoTexs::DISPLACEMENT},
                                                               {aiTextureType_LIGHTMAP, VoTexs::LIGHTMAP},
                                                               {aiTextureType_REFLECTION, VoTexs::REFLECTION},
                                                               {aiTextureType_UNKNOWN, VoTexs::UNKNOWN}};

//---------------------------------------------------------------------------------------------------------------------
constexpr auto AssimpTexTypes = {aiTextureType_DIFFUSE,   aiTextureType_SPECULAR,   aiTextureType_AMBIENT,
                                 aiTextureType_EMISSIVE,  aiTextureType_HEIGHT,     aiTextureType_NORMALS,
                                 aiTextureType_SHININESS, aiTextureType_OPACITY,    aiTextureType_DISPLACEMENT,
                                 aiTextureType_LIGHTMAP,  aiTextureType_REFLECTION, aiTextureType_UNKNOWN};
//---------------------------------------------------------------------------------------------------------------------
struct AssimpMaterial {
  aiString   name{""};
  float      shine{0.f}, shineI{0.f}, reflectivity{0.f}, refraction{0.f};
  aiColor4D  diffuse{1.f}, ambient{1.f}, specular{1.f}, emissive{1.f};
  glm::vec4  ai4(aiColor4D const &c) { return {c.r, c.g, c.b, c.a}; };
  VoMaterial toVoMaterial() {
    return VoMaterial{name.C_Str(), shine,        shineI,        reflectivity, refraction,
                      ai4(diffuse), ai4(ambient), ai4(specular), ai4(emissive)};
  }
};


//---------------------------------------------------------------------------------------------------------------------
//--- MESH Helpers
//---------------------------------------------------------------------------------------------------------------------
void copyMeshAttrib(std::vector<unsigned int> &out, aiFace const *in, unsigned int numFaces) { // Faces
  out.reserve(numFaces * 3u);
  for (auto i = 0u; i < numFaces; ++i)
    for (auto j = 0u; (in[i].mNumIndices == 3 and j < in[i].mNumIndices); ++j) { out.emplace_back(in[i].mIndices[j]); }
}
//---------------------------------------------------------------------------------------------------------------------
void copyMeshAttrib(std::vector<glm::vec2> &out, aiVector3D const *in, unsigned int numVertices) { // Textures
  out.reserve(numVertices);
  for (auto i = 0u; i < numVertices; ++i) { out.emplace_back(in[i].x, in[i].y); }
}
//---------------------------------------------------------------------------------------------------------------------
void copyMeshAttrib(std::vector<glm::vec3> &out, aiVector3D const *in, unsigned int numVertices) { // Others
  out.reserve(numVertices);
  for (auto i = 0u; i < numVertices; ++i) { out.emplace_back(in[i].x, in[i].y, in[i].z); }
}
//---------------------------------------------------------------------------------------------------------------------
RenderablePOD meshDigest(aiMesh const *mesh) {
  RenderablePOD R;
  copyMeshAttrib(R.indices, mesh->mFaces, mesh->mNumFaces);
  copyMeshAttrib(R.positions, mesh->mVertices, mesh->mNumVertices);
  if (mesh->HasNormals()) { copyMeshAttrib(R.normals, mesh->mNormals, mesh->mNumVertices); }
  if (mesh->HasTangentsAndBitangents()) {
    copyMeshAttrib(R.tangents, mesh->mTangents, mesh->mNumVertices);
    copyMeshAttrib(R.bitangents, mesh->mBitangents, mesh->mNumVertices);
  }
  for (auto j = 0u; (!R.texCoords.size() and j < AI_MAX_NUMBER_OF_TEXTURECOORDS); ++j) {
    if (!mesh->HasTextureCoords(j)) { continue; }
    copyMeshAttrib(R.texCoords, mesh->mTextureCoords[j], mesh->mNumVertices);
  }
  return R;
}


//---------------------------------------------------------------------------------------------------------------------
//--- ASSIMP PARSER
// http://assimp.sourceforge.net/lib_html/postprocess_8h.html#aaa612ed3f167a8999405d53fc309594b
// http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a6afb4fee42eca4482674859196cb8685
// @DANI: if (scene->HasAnimations() or scene->HasLights() or scene->HasCameras() or scene->HasTextures()) {}
//---------------------------------------------------------------------------------------------------------------------
RenderableGroup import(std::string const &filePath) {
  RenderableGroup RG{filePath};

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  Assimp::Importer importer;
  importer.SetPropertyFloat("AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE", 80.f);

  aiScene const *scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);
  if (!scene or scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE or !scene->mRootNode) {
    vo_err("[ASSIMP] : {}", importer.GetErrorString());
    return RG;
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  //*Insert here the logic for: Move MeshFile from load-path to project-path*

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  if (scene->HasMeshes()) {
    // vo_log("*** Processing mesh '{}' ***", filePath);
    for (auto i = 0u; i < scene->mNumMeshes; ++i) {
      auto const name = vo_fmt("{}_{}", filePath, i);
      auto const data = meshDigest(scene->mMeshes[i]);
      RG.addRenderable(name, data);
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  if (scene->HasMaterials()) {
    std::vector<aiMaterial> materials;
    materials.reserve(scene->mNumMaterials);
    for (auto i = 0u; i < scene->mNumMaterials; ++i) {
      auto &&        mat = scene->mMaterials[i];
      AssimpMaterial aiMat;

      // Props
      //..................................................
      mat->Get(AI_MATKEY_NAME, aiMat.name);
      mat->Get(AI_MATKEY_SHININESS, aiMat.shine);
      mat->Get(AI_MATKEY_SHININESS_STRENGTH, aiMat.shineI);
      mat->Get(AI_MATKEY_REFLECTIVITY, aiMat.reflectivity);
      mat->Get(AI_MATKEY_REFRACTI, aiMat.refraction);
      mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiMat.diffuse);
      mat->Get(AI_MATKEY_COLOR_AMBIENT, aiMat.ambient);
      mat->Get(AI_MATKEY_COLOR_SPECULAR, aiMat.specular);
      mat->Get(AI_MATKEY_COLOR_EMISSIVE, aiMat.emissive);
      auto voMat = aiMat.toVoMaterial();
      //..................................................

      // Textures
      //..................................................
      //*std::unordered_map<std::string, std::string> texturesPaths;
      for (auto texType : AssimpTexTypes) {
        if (int totalTexOfCurrType = mat->GetTextureCount(texType); totalTexOfCurrType > 0) {
          aiString texturePath;
          for (auto j = 0; j < totalTexOfCurrType; ++j) {
            if (mat->GetTexture(texType, j, &texturePath) != AI_SUCCESS) continue;
            //*Insert here the logic for: Move texture from load-path to project-path and save the new on voMat*
            //*texturesPaths.emplace(texturePath.C_Str(), newTexturePath);
            voMat.texPaths.at(VoAssimpTex.at(texType)) = texturePath.C_Str();
          }
        }
      }
      //..................................................

      // Append material to
      //..................................................
      RG.addMaterial(voMat);
      //..................................................
      // (i == 0) ? vo_print("\n=== Processing material {} ===", mat->GetName().C_Str(), i)
      //          : vo_print("=== Processing material {} ===", mat->GetName().C_Str(), i);
      // voMat.info();
      // vo_print("=== / Processing material {} ===\n", mat->GetName().C_Str(), i);
      //..................................................

      // Clean if MTL
      //..................................................
      //*Insert here the logic for: Save MTL with fixed texture paths to be relative*
      //..................................................
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  // THEN COMPOSE THE HIERARCHY OF MESHES //? Responsibiity moved to RenderableGroup ?
  // auto &&root = scene->mRootNode;

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


  return RG;
}

} // namespace Vonsai::Mesh
