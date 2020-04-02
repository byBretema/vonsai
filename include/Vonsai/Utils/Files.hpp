#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace Vonsai::Files {

bool        isValid(std::string const &path, bool log = true);
std::string fromString(std::string const &filePath);

} // namespace Vonsai::Files
