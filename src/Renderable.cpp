#include <Vonsai/Camera.hpp>
#include <Vonsai/Renderable.hpp>
#include <Vonsai/Shader.hpp>
// #include <Vonsai/Texture.hpp>

#include <Vonsai/Utils/Logger.hpp>
#include <Vonsai/Wraps/_gl.hpp>

#include <mutex>

#include <typeinfo>

namespace Vonsai {


void VoMaterial::info() const {
  vo_log(" NAME: {}", name);
  vo_log(" SHININESS: {}", shine);
  vo_log(" SHININESS_STRENGTH: {}", shineI);
  vo_log(" REFLECTIVITY: {}", reflectivity);
  vo_log(" REFRACTION: {}", refraction);
  vo_log(" COLOR DIFFUSE:  ( {}, {}, {}, {} )", diffuse.r, diffuse.g, diffuse.b, diffuse.a);
  vo_log(" COLOR AMBIENT:  ( {}, {}, {}, {} )", ambient.r, ambient.g, ambient.b, ambient.a);
  vo_log(" COLOR SPECULAR: ( {}, {}, {}, {} )", specular.r, specular.g, specular.b, specular.a);
  vo_log(" COLOR EMISSIVE: ( {}, {}, {}, {} )", emissive.r, emissive.g, emissive.b, emissive.a);
  if (!texPaths.at(VoTexs::DIFFUSE).empty()) vo_log(" TEXTURE DIFFUSE: {}", texPaths.at(VoTexs::DIFFUSE));
  if (!texPaths.at(VoTexs::SPECULAR).empty()) vo_log(" TEXTURE SPECULAR: {}", texPaths.at(VoTexs::SPECULAR));
  if (!texPaths.at(VoTexs::AMBIENT).empty()) vo_log(" TEXTURE AMBIENT: {}", texPaths.at(VoTexs::AMBIENT));
  if (!texPaths.at(VoTexs::EMISSIVE).empty()) vo_log(" TEXTURE EMISSIVE: {}", texPaths.at(VoTexs::EMISSIVE));
  if (!texPaths.at(VoTexs::HEIGHT).empty()) vo_log(" TEXTURE HEIGHT: {}", texPaths.at(VoTexs::HEIGHT));
  if (!texPaths.at(VoTexs::NORMALS).empty()) vo_log(" TEXTURE NORMALS: {}", texPaths.at(VoTexs::NORMALS));
  if (!texPaths.at(VoTexs::SHININESS).empty()) vo_log(" TEXTURE SHININESS: {}", texPaths.at(VoTexs::SHININESS));
  if (!texPaths.at(VoTexs::OPACITY).empty()) vo_log(" TEXTURE OPACITY: {}", texPaths.at(VoTexs::OPACITY));
  if (!texPaths.at(VoTexs::DISPLACEMENT).empty()) vo_log(" TEXTURE DISPL: {}", texPaths.at(VoTexs::DISPLACEMENT));
  if (!texPaths.at(VoTexs::LIGHTMAP).empty()) vo_log(" TEXTURE LIGHTMAP: {}", texPaths.at(VoTexs::LIGHTMAP));
  if (!texPaths.at(VoTexs::REFLECTION).empty()) vo_log(" TEXTURE REFLECTION: {}", texPaths.at(VoTexs::REFLECTION));
}

//---------------------------------------------------------------------------------------------------------------------

Renderable::Renderable(std::string const &name, RenderablePOD const &a_data) {
  m_name = name;
  if (s_uniqueRenderableNames.count(name) > 0) {
    ++s_uniqueRenderableNames.at(name);
    m_name += "_" + std::to_string(s_uniqueRenderableNames.at(name));
  } else {
    s_uniqueRenderableNames.emplace(name, 0);
  }

  if (a_data.indices.size() < 3 or a_data.positions.size() < 3) { return; }

  GL_ASSERT(glGenVertexArrays(1, &m_VAO));
  setEBO(a_data.indices);
  addVBO(a_data.positions); // 0

  std::vector<glm::vec3> dummy3(a_data.indices.size(), {0, 0, 0});
  std::vector<glm::vec2> dummy2(a_data.indices.size(), {0, 0});
  // Safe empty cases
  (a_data.normals.size()) ? addVBO(a_data.normals) : addVBO(dummy3);       // 1
  (a_data.texCoords.size()) ? addVBO(a_data.texCoords) : addVBO(dummy2);   // 2
  (a_data.tangents.size()) ? addVBO(a_data.tangents) : addVBO(dummy3);     // 3
  (a_data.bitangents.size()) ? addVBO(a_data.bitangents) : addVBO(dummy3); // 4

  m_valid      = true;
  m_indexCount = a_data.indices.size();
}

// --------------------------------------------------------------------------------------------------------------------

DC_ALLOW_MOVE_C(                                          //
    Renderable,                                           //
    swap(lhs.m_name, rhs.m_name);                         //
    swap(lhs.m_valid, rhs.m_valid);                       //
    swap(lhs.m_alertOnceInvalid, rhs.m_alertOnceInvalid); //
    swap(lhs.m_alertOnceDraw, rhs.m_alertOnceDraw);       //
    swap(lhs.m_location, rhs.m_location);                 //
    swap(lhs.m_VAO, rhs.m_VAO);                           //
    swap(lhs.m_EBO, rhs.m_EBO);                           //
    swap(lhs.m_indexCount, rhs.m_indexCount);             //
);

// --------------------------------------------------------------------------------------------------------------------

void Renderable::draw() const {
  if (!m_alertOnceInvalid and !m_valid) {
    vo_err("Renderable '{}' : Something wrong in creation", m_name);
    m_alertOnceInvalid = true;
  } else if (!m_alertOnceDraw and m_indexCount < 3) {
    vo_err("Renderable '{}' : Not enough data to draw", m_name);
    m_alertOnceDraw = true;
  } else {
    BindGuard bgR{*this};
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
  }
}

// --------------------------------------------------------------------------------------------------------------------

bool Renderable::isValid() const { return m_valid; }

// --------------------------------------------------------------------------------------------------------------------

void Renderable::setEBO(std::vector<unsigned int> const &a_data) {
  if (m_indexCount) {
    vo_err("Renderable EBO Data only can be setted one time");
    return;
  }

  GL_ASSERT(glBindVertexArray(m_VAO));
  {
    GL_ASSERT(glGenBuffers(1, &m_EBO));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_data.size() * sizeof(a_data[0]), &a_data[0], GL_STATIC_DRAW));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  }
  GL_ASSERT(glBindVertexArray(0));
}

// --------------------------------------------------------------------------------------------------------------------

#define d_addVBO(a_data, a_dataSize, a_type)                                                               \
  BindGuard    bg{*this};                                                                                  \
  unsigned int m_VBO;                                                                                      \
  GL_ASSERT(glGenBuffers(1, &m_VBO));                                                                      \
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));                                                         \
  GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, a_data.size() * sizeof(a_data[0]), &a_data[0], GL_STATIC_DRAW)); \
  GL_ASSERT(glEnableVertexAttribArray(m_location));                                                        \
  GL_ASSERT(glVertexAttribPointer(m_location, a_dataSize, a_type, GL_FALSE, 0, 0));                        \
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));                                                             \
  ++m_location;

void Renderable::addVBO(std::vector<float> const &a_data, int a_dataSize) { d_addVBO(a_data, a_dataSize, GL_FLOAT); }
void Renderable::addVBO(std::vector<glm::vec3> const &a_data) { d_addVBO(a_data, a_data[0].length(), GL_FLOAT); }
void Renderable::addVBO(std::vector<glm::vec2> const &a_data) { d_addVBO(a_data, a_data[0].length(), GL_FLOAT); }

// --------------------------------------------------------------------------------------------------------------------

void Renderable::bind() const {
  GL_ASSERT(glBindVertexArray(m_VAO));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
}

// --------------------------------------------------------------------------------------------------------------------

void Renderable::unbind() const {
  GL_ASSERT(glBindVertexArray(0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


//---------------------------------------------------------------------------------------------------------------------
//--- RENDERABLE GROUP
//---------------------------------------------------------------------------------------------------------------------

DC_ALLOW_MOVE_C(                                                              //
    RenderableGroup,                                                          //
    swap(lhs.m_name, rhs.m_name);                                             //
    swap(lhs.m_materials, rhs.m_materials);                                   //
    swap(lhs.m_renderables, rhs.m_renderables);                               //
    swap(lhs.m_textureCache, rhs.m_textureCache);                             //
    swap(lhs.m_relationRenderableMaterial, rhs.m_relationRenderableMaterial); //
);

// --------------------------------------------------------------------------------------------------------------------

Transform *RenderableGroup::transform(unsigned int idx) {
  return (idx < m_renderables.size()) ? &m_renderables.at(idx).transform : nullptr;
}

// --------------------------------------------------------------------------------------------------------------------

void RenderableGroup::addMaterial(VoMaterial &material) {
  auto  matIdx = m_materials.size();
  auto &M      = m_materials.emplace_back(material);
  for (auto &[texType, texPath] : M.texPaths) {
    // TODO: @DANI : Fix paths issue...
    if (!texPath.empty()) {
      // vo_log("1 - {} >>>> {}/{} ", m_name, matIdx, VoTexsStr.at(texType));
      m_textureCache.emplace(matIdx, std::make_pair(      //
                                         texType,         //
                                         Texture{texPath} // !! fails here (with relative path)
                                         ));
    }
  }
}

// --------------------------------------------------------------------------------------------------------------------

void RenderableGroup::addRenderable(std::string const &name, RenderablePOD const &data, int matIdx) {
  m_renderables.emplace_back(name, data);
  if (matIdx > 0) { m_relationRenderableMaterial.emplace(name, matIdx); }
}

// --------------------------------------------------------------------------------------------------------------------

void RenderableGroup::drawAsPBR(Shader const &S, Camera const &cam) {
  for (auto &&R : m_renderables) {
    // vo_log("DRAWING [{} - ]", m_name, R.getName());

    auto const matIdx =
        (m_relationRenderableMaterial.count(R.getName()) > 0) ? m_relationRenderableMaterial[R.getName()] : -1;

    BindGuard bgR{R};
    BindGuard bgS{S};

    // Upload camera to the shader
    auto const modelView = cam.getView() * R.transform.matrix();
    auto const normalMat = glm::transpose(glm::inverse(modelView));
    S.setMat4("u_modelView", modelView);
    S.setMat4("u_normalMat", normalMat);

    if (matIdx >= 0) {
      auto M = m_materials[matIdx];

      // Upload mat props to the shader
      S.setFloat1("u_Shine", M.shine);
      S.setFloat1("u_Refraction", M.refraction);
      // S.setFloat1("u_Reflectivity", M.reflectivity);
      // S.setFloat1("u_ShineIntensity", M.shineI);
      S.setFloat4("u_ColorDiffuse", M.diffuse);
      S.setFloat4("u_ColorSpecular", M.specular);
      S.setFloat4("u_ColorAmbient", M.ambient);
      S.setFloat4("u_ColorEmissive", M.emissive);

      // Upload mat textures to the shader
      auto range = m_textureCache.equal_range(matIdx);
      for_each(range.first, range.second, [&S](auto &TexData) {
        auto &&     texType = TexData.second.first;
        auto const *T       = &TexData.second.second;
        // vo_log("@@@@ {} / {}", VoTexsStr.at(texType), T->getID());
        T->bind(); // Revisit this to check IDs if it fails.
        switch (texType) {
          case VoTexs::DIFFUSE: S.setTexture("u_TexDiffuse", T->getID()); break;
          case VoTexs::SPECULAR: S.setTexture("u_TexSpecular", T->getID()); break;
          case VoTexs::AMBIENT: S.setTexture("u_TexAmbient", T->getID()); break;
          case VoTexs::EMISSIVE: S.setTexture("u_TexEmissive", T->getID()); break;
          // case VoTexs::HEIGHT: S.setTexture("u_TexHeight", T->getID()); break;
          // case VoTexs::SHININESS: S.setTexture("u_TexShininess", T->getID()); break;
          // case VoTexs::OPACITY: S.setTexture("u_TexOpacity", T->getID()); break;
          // case VoTexs::DISPLACEMENT: S.setTexture("u_TexDisplacement", T->getID()); break;
          // case VoTexs::LIGHTMAP: S.setTexture("u_TexLightmap", T->getID()); break;
          // case VoTexs::REFLECTION: S.setTexture("u_TexReflection", T->getID()); break;
          default: break;
        }
      });
    } // if (matIdx >= 0)


    // Draw call
    R.draw();

    // Unbind textures
    if (matIdx >= 0) {
      auto range = m_textureCache.equal_range(matIdx);
      for_each(range.first, range.second, [](auto &TexData) { TexData.second.second.unbind(); });
    }

  } // for (auto &&R : m_renderables)
}

// --------------------------------------------------------------------------------------------------------------------

} // namespace Vonsai
