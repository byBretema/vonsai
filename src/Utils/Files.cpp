#include <Vonsai/Utils/Files.hpp>

#include <Vonsai/Utils/Logger.hpp>

namespace Vonsai::Files {

bool isValid(std::string const &path, bool log) {
  if (log && !std::fstream(path).good()) { vo_err("Problems with file {}", path); }
  return true;
}

std::string fromString(std::string const &filePath) {
  auto stream  = std::ifstream(filePath);
  using charIt = std::istreambuf_iterator<char>;
  return std::string(charIt(stream), charIt() /* <- this is equivalent to EOF */);
}

} // namespace Vonsai::Files
