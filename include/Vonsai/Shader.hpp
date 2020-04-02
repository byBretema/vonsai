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
  unsigned int m_programID   = 0u;
  char const * m_programName = "";

  mutable bool m_ok    = true;
  mutable bool m_bound = false;
  mutable bool m_built = false;

  mutable std::unordered_map<char const *, int>  m_uniformCache;
  mutable std::unordered_map<char const *, bool> m_uniformAlertCache;

  mutable std::unordered_map<char const *, int>  m_uniformBlockCache;
  mutable std::unordered_map<char const *, bool> m_uniformBlockAlertCache;

  void buildPipeline(ShaderCode const &a_rawCode);

public:
  inline bool isReady() const { return m_ok && m_built && m_bound; }
  int         getUniformLocation(char const *a_name) const;

  void bind() const override;
  void unbind() const override;

  void setUniformMat4(char const *a_name, glm::mat4 const &a_mat) const;
  void setUniformFloat1(char const *a_name, float a_float) const;
  void setUniformFloat3(char const *a_name, float a_f1, float a_f2, float a_f3) const;
  void setUniformFloat3(char const *a_name, glm::vec3 const &a_floats) const;
  void setUniformInt1(char const *a_name, int a_int) const;
  void setUniformBlock(char const *a_name, int uboBindPoint) const;

  Shader(char const *a_name, ShaderPath const &a_paths);
  Shader(char const *a_name, ShaderCode const &a_rawCode);
  Shader(Shader &&) = delete;
  Shader &operator=(Shader &&) = delete;
  Shader(Shader const &)       = delete;
  Shader &operator=(Shader const &) = delete;
};

} // namespace Vonsai
