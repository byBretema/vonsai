#pragma once

#include <Vonsai/Renderable.hpp>
#include <string>

namespace Vonsai {

std::vector<RenderablePOD> getMeshFromFile(std::string const &filePath);

} // namespace Vonsai
