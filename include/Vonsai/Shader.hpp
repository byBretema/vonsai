#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include "Bindable.hpp"
#include "Wraps/_glm.hpp"

namespace Vonsai {

class ShaderPath {
private:
  std::string defValue = "";

public:
  std::string vertex   = "";
  std::string tessC    = "";
  std::string tessE    = "";
  std::string geometry = "";
  std::string fragment = "";

  std::string &operator[](unsigned int i) { //
    return const_cast<std::string &>(std::as_const(*this).operator[](i));
  }

  std::string const &operator[](unsigned int i) const {
    switch (i) {
      case 0: return vertex;
      case 1: return tessC;
      case 2: return tessE;
      case 3: return geometry;
      case 4: return fragment;
      default: return defValue;
    }
  }
};

class ShaderCode {
private:
  std::string defValue = "";

public:
  std::string vertex   = "";
  std::string tessC    = "";
  std::string tessE    = "";
  std::string geometry = "";
  std::string fragment = "";

  std::string &operator[](unsigned int i) { //
    return const_cast<std::string &>(std::as_const(*this).operator[](i));
  }

  std::string const &operator[](unsigned int i) const {
    switch (i) {
      case 0: return vertex;
      case 1: return tessC;
      case 2: return tessE;
      case 3: return geometry;
      case 4: return fragment;
      default: return defValue;
    }
  }
};

class Shader : public Bindable {
private:
  unsigned int programID   = 0u;
  char const * programName = "";

  mutable bool ok    = true;
  mutable bool bound = false;
  mutable bool built = false;

  mutable std::unordered_map<char const *, int>  uniformCache;
  mutable std::unordered_map<char const *, bool> uniformAlertCache;

  mutable std::unordered_map<char const *, int>  uniformBlockCache;
  mutable std::unordered_map<char const *, bool> uniformBlockAlertCache;

public:
  explicit Shader(char const *name);
  Shader(char const *name, ShaderPath const &paths);
  Shader(char const *name, ShaderCode const &rawCode);
  Shader(Shader &&) = delete;
  Shader &operator=(Shader &&) = delete;
  Shader(Shader const &)       = delete;
  Shader &operator=(Shader const &) = delete;

  void buildPipeline(ShaderPath const &paths);
  void buildPipeline(ShaderCode const &rawCode);

  inline bool isReady() const { return ok && built && bound; }

  void unbind() const override;
  void bind() const override;

  int  getUniformLocation(char const *name) const;
  void setUniformMat4(char const *name, glm::mat4 const &mat) const;
  void setUniformFloat1(char const *name, float f) const;
  void setUniformFloat3(char const *name, float f1, float f2, float f3) const;
  void setUniformFloat3(char const *name, glm::vec3 const &floats) const;
  void setUniformInt1(char const *name, int i) const;

  void setUniformBlock(char const *name, int uboBindPoint) const;
};

} // namespace Vonsai
