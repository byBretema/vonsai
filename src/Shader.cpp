#include <Vonsai/Shader.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Logger.hpp>

#include <Vonsai/Wraps/_gl.hpp>

#include <vector>

namespace Vonsai {

std::array<const char *, 5> constexpr SHADER_NAMES{
    "VERTEX",       //
    "TESS_CONTROL", //
    "TESS_EVAL",    //
    "GEOMETRY",     //
    "FRAGMENT"      //
};

std::array<int, 5> constexpr SHADER_TYPES{
    GL_VERTEX_SHADER,          //
    GL_TESS_CONTROL_SHADER,    //
    GL_TESS_EVALUATION_SHADER, //
    GL_GEOMETRY_SHADER,        //
    GL_FRAGMENT_SHADER         //
};

// CONSTRUCTOR <- Files
Shader::Shader(std::string const &a_name, ShaderPath const &a_paths) : m_programName(a_name) {
  ShaderCode rawCode;

  for (auto i = 0u; i < SHADER_TYPES.size(); ++i) {
    if (a_paths[i].empty() or !Files::isValid(a_paths[i])) {
      if (SHADER_TYPES[i] == GL_VERTEX_SHADER or SHADER_TYPES[i] == GL_FRAGMENT_SHADER) {
        vo_err("ShaderPaths looks incorrect at some point");
        m_valid = false;
        return;
      }
      continue;
    }
    rawCode[i] = Files::fromString(a_paths[i]);
  }

  buildPipeline(rawCode);
}

// CONSTRUCTOR <- Code as string
Shader::Shader(std::string const &a_name, ShaderCode const &a_rawCode) : m_programName(a_name) {
  buildPipeline(a_rawCode);
}

std::string const &Shader::getName() const { return m_programName; }

bool Shader::isReady() const { return m_valid && m_built; }

void Shader::bind() const { GL_ASSERT(glUseProgram(m_programID)); }

void Shader::unbind() const { GL_ASSERT(glUseProgram(0)); }

/// Returns the ID of the uniform associated to that string,
/// if its cached, return from cache, else request it to OpenGL
/// and store on cache.
inline int Shader::getUniformLocation(std::string const &a_name) const {
  if (m_uniformCache.count(a_name) > 0) { return m_uniformCache.at(a_name); }
  int uniformLocation = glGetUniformLocation(m_programID, a_name.c_str());

  if (uniformLocation > -1) {
    m_uniformCache.emplace(a_name, uniformLocation);
  } else if (!m_uniformAlertCache[a_name]) {
    vo_log("({}) Not found/used uniform: {}", m_programName, a_name);
    m_uniformAlertCache[a_name] = true;
  }

  return uniformLocation;
}

// SET UNIFORMS
void Shader::setTexture(std::string const &a_name, int a_int) const {
  glProgramUniform1i(m_programID, getUniformLocation(a_name), a_int);
}
void Shader::setFloat1(std::string const &a_name, float a_float) const {
  glProgramUniform1f(m_programID, getUniformLocation(a_name), a_float);
}
void Shader::setFloat3(std::string const &a_name, glm::vec3 const &a_floats) const {
  glProgramUniform3fv(m_programID, getUniformLocation(a_name), 1, glm::value_ptr(a_floats));
}
void Shader::setMat4(std::string const &a_name, glm::mat4 const &a_mat) const {
  glProgramUniformMatrix4fv(m_programID, getUniformLocation(a_name), 1, GL_FALSE, glm::value_ptr(a_mat));
}

// Connect a given UBO
void Shader::linkUBO(std::string const &a_name, unsigned int a_uboBindPoint) const {
  int ubShaderIdx = 0;

  if (m_uniformBlockCache.count(a_name) > 0) {
    ubShaderIdx = m_uniformBlockCache.at(a_name);

  } else {
    ubShaderIdx = glGetUniformBlockIndex(m_programID, a_name.c_str());

    if (ubShaderIdx > -1) {
      m_uniformBlockCache.emplace(a_name, ubShaderIdx);

    } else if (!m_uniformBlockAlertCache[a_name]) {
      vo_log("({}) Not found uniform_block: {}", m_programName, a_name);
      m_uniformBlockAlertCache[a_name] = true;
      return;
    }
  }

  glUniformBlockBinding(m_programID, ubShaderIdx, a_uboBindPoint);
}

// Build pipeline from code as string
void Shader::buildPipeline(ShaderCode const &a_rawCode) {
  if (m_built) {
    vo_err("Shader '{}' is already built.", m_programName);
    return;
  };

  // Request program to opengl
  m_programID = glCreateProgram();

  auto onError = [&]() {
    glDeleteProgram(m_programID);
    m_programID = 0;
    m_valid     = false;
  };

  for (auto i = 0u; i < SHADER_TYPES.size(); ++i) {

    if (a_rawCode[i].empty()) {
      if (SHADER_TYPES[i] == GL_VERTEX_SHADER or SHADER_TYPES[i] == GL_FRAGMENT_SHADER) {
        onError();
        vo_err("Empty code for Vertex or Fragment shaders");
        return;
      }
      continue;
    }

    char const *sourceChar = a_rawCode[i].c_str();
    int         sourceLen  = a_rawCode[i].length();

    // Request shader to opengl
    unsigned int const shaderID = glCreateShader(SHADER_TYPES[i]);
    GL_ASSERT(glShaderSource(shaderID, 1, &sourceChar, &sourceLen));
    GL_ASSERT(glCompileShader(shaderID));

    // Check errors
    {
      GLint compiled;
      glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
      if (compiled) {
        glAttachShader(m_programID, shaderID);
      } else {

        int len;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> msg(len);
        glGetShaderInfoLog(shaderID, len, &len, &msg[0]);

        vo_err("SHADER ({}) - {}\n{}", SHADER_NAMES[i], m_programName, std::string_view(msg.data(), msg.size()));

        onError();
        return;
      }
    }
    // Always remove the shader
    glDeleteShader(shaderID);
  }

  // Link the program
  GL_ASSERT(glLinkProgram(m_programID));

  // Check errors
  {
    int linked;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &linked);
    if (linked) {
      m_built = true;
    } else {

      int len = 0;
      glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &len);
      std::vector<char> msg(len);
      glGetProgramInfoLog(m_programID, len, &len, &msg[0]);

      vo_err("PROGRAM {}\n{}", m_programName, std::string_view(msg.data(), msg.size()));

      onError();
      return;
    }
  }
}

} // namespace Vonsai
