#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include "Bindable.hpp"
#include "Wraps/_glm.hpp"

#include "Utils/Logger.hpp"

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

  ShaderPath(std::string const &v, std::string const &f, std::string const &g = "", std::string const &tc = "",
             std::string const &te = "") {
    vertex   = v;
    tessC    = tc;
    tessE    = te;
    geometry = g;
    fragment = f;
  }

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

  ShaderCode(std::string const &v = "", std::string const &f = "", std::string const &g = "",
             std::string const &tc = "", std::string const &te = "") {
    vertex   = v;
    tessC    = tc;
    tessE    = te;
    geometry = g;
    fragment = f;
  }

  bool isValid() { return !vertex.empty() && !fragment.empty(); }

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
public:
  Shader(std::string const &a_name, ShaderPath const &a_paths);
  Shader(std::string const &a_name, ShaderCode const &a_rawCode);

  // Not copiable
  Shader(Shader const &rhs) = delete;
  Shader &operator=(Shader const &rhs) = delete;

  // Move semantics
  Shader(Shader &&rhs) noexcept;
  Shader &    operator=(Shader &&rhs) noexcept;
  friend void swap(Shader &lhs, Shader &rhs) noexcept;

  /** Verify that the Shader is built and in a valid state. */
  bool isReady() const;

  /** Returns the ID of the uniform associated to that string,
  * if its cached, return from cache, else request it to OpenGL
  * and store on cache. */
  int getUniformLocation(std::string const &a_name) const;

  void setTexture(std::string const &a_name, int a_int) const;
  void setFloat1(std::string const &a_name, float a_float) const;
  void setFloat3(std::string const &a_name, glm::vec3 const &a_floats) const;
  void setFloat4(std::string const &a_name, glm::vec4 const &a_floats) const;
  void setMat4(std::string const &a_name, glm::mat4 const &a_mat) const;

  void linkUBO(std::string const &a_name, unsigned int uboBindPoint) const;

  void bind() const override;
  void unbind() const override;

private:
  unsigned int m_programID{0u};
  std::string  m_programName{""};

  mutable bool m_valid{false};
  mutable bool m_built{false};

  mutable std::unordered_map<std::string, int>  m_uniformCache{};
  mutable std::unordered_map<std::string, bool> m_uniformAlertCache{};

  mutable std::unordered_map<std::string, int>  m_uniformBlockCache{};
  mutable std::unordered_map<std::string, bool> m_uniformBlockAlertCache{};

  explicit Shader(std::string const &a_name);

  void buildPipeline(ShaderCode const &a_rawCode);
};

} // namespace Vonsai
