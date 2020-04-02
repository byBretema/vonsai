#include <Vonsai/Utils/Strings.hpp>

namespace Vonsai::Strings {

std::vector<std::string> split(std::string const &str, std::string const &delimeter) {
  std::vector<std::string> splitted;

  std::string token;
  size_t      ini{0}, end{0};

  // Split and store the string body
  while ((end = str.find(delimeter, ini)) <= str.size()) {
    token = str.substr(ini, end - ini);
    ini   = end + delimeter.size();
    splitted.push_back(token);
  }

  // Store the string tail
  if (ini < str.size()) {
    token = str.substr(ini);
    splitted.push_back(token);
  }

  return splitted;
}

} // namespace Vonsai::Strings
