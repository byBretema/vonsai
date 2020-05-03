#pragma once

#include "../Renderable.hpp"
#include <string>
#include <vector>

namespace Vonsai::Mesh {

std::vector<RenderablePOD> import(std::string const &filePath);

} // namespace Vonsai::Mesh
