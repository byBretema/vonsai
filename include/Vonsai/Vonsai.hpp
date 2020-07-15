#pragma once

#include "IO/Input.hpp"
#include "IO/Window.hpp"

#include "Utils/Colors.hpp"
#include "Utils/Logger.hpp"
#include "Utils/Mesh.hpp"
#include "Utils/Timer.hpp"

#include "Bindable.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "Light.hpp"
#include "Renderable.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "UBO.hpp"

#include "Wraps/_globals.hpp"

#include <imgui/imgui.h>

namespace vo = Vonsai;

inline void vo_draw(vo::RenderableGroup &RG, vo::Shader const &S, vo::Texture const *T, vo::Camera const &cam,
                    bool active) {
  if (!active) { return; }
  for (auto &&R : RG.group()) {
    Vonsai::BindGuard bgR{R};
    Vonsai::BindGuard bgS{S};
    Vonsai::BindGuard bgT{T};

    if (T) { S.setTexture("u_texture", T->getID()); }

    auto const modelView = cam.getView() * R.transform.matrix();
    auto const normalMat = glm::transpose(glm::inverse(modelView));

    S.setMat4("u_modelView", modelView);
    S.setMat4("u_normalMat", normalMat);

    R.draw();
  }
};
