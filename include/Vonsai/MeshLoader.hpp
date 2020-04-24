#pragma once

#include <Vonsai/Renderable.hpp>
#include <string>

namespace Vonsai {

RenderablePOD getMeshFromOBJ(std::string const &filePath);
RenderablePOD getMeshFromFile(std::string const &filePath);

} // namespace Vonsai
