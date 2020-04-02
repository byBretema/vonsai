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

void Shader::bind() const {
  GL_ASSERT(glUseProgram(programID));
  bound = true;
}
void Shader::unbind() const {
  GL_ASSERT(glUseProgram(0));
  bound = false;
}
/// Ctor just name
Shader::Shader(char const *name) : programName(name) {}
/// Ctor from files
Shader::Shader(char const *name, ShaderPath const &paths) : Shader(name) { buildPipeline(paths); }
/// Ctor from code as string
Shader::Shader(char const *name, ShaderCode const &rawCode) : Shader(name) { buildPipeline(rawCode); }

/// Build pipeline from files
void Shader::buildPipeline(ShaderPath const &paths) {
  if (built) {
    vo_err("Shader '{}' is already built.", programName);
    return;
  };

  ShaderCode rawCode;

  for (auto i = 0; i < SHADER_TYPES.size(); ++i) {
    if (paths[i].empty() or !Files::isValid(paths[i])) {
      if (SHADER_TYPES[i] == GL_VERTEX_SHADER or SHADER_TYPES[i] == GL_FRAGMENT_SHADER) { ok = false; }
      continue;
    }
    auto const code = Files::fromString(paths[i]);
    rawCode[i]      = code;
  }

  buildPipeline(rawCode);
}

/// Build pipeline from code as string
void Shader::buildPipeline(ShaderCode const &rawCode) {
  if (!ok) {
    vo_err("ShaderPaths looks incorrect at some point");
    return;
  };
  if (built) {
    vo_err("Shader '{}' is already built.", programName);
    return;
  };

  // Request program to opengl
  programID = glCreateProgram();

  auto onError = [&]() {
    glDeleteProgram(programID);
    programID = 0;
    ok        = false;
  };

  for (auto i = 0; i < SHADER_TYPES.size(); ++i) {

    if (rawCode[i].empty()) {
      if (SHADER_TYPES[i] == GL_VERTEX_SHADER or SHADER_TYPES[i] == GL_FRAGMENT_SHADER) {
        onError();
        vo_err("Empty code for Vertex or Fragment shaders");
        return;
      }
      continue;
    }

    char const *sourceChar = rawCode[i].c_str();
    int         sourceLen  = rawCode[i].length();

    // Request shader to opengl
    unsigned int const shaderID = glCreateShader(SHADER_TYPES[i]);
    GL_ASSERT(glShaderSource(shaderID, 1, &sourceChar, &sourceLen));
    GL_ASSERT(glCompileShader(shaderID));

    // Check errors
    {
      GLint compiled;
      glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
      if (compiled) {
        glAttachShader(programID, shaderID);
      } else {
        onError();

        int len;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> msg(len);
        glGetShaderInfoLog(shaderID, len, &len, &msg[0]);

        vo_err("SHADER ({}) - {}\n{}", SHADER_NAMES[i], programName, std::string_view(msg.data(), msg.size()));

        return;
      }
    }
    // Always remove the shader
    glDeleteShader(shaderID);
  }

  // Link the program
  GL_ASSERT(glLinkProgram(programID));

  // Check errors
  {
    int linked;
    glGetProgramiv(programID, GL_LINK_STATUS, &linked);
    if (linked) {
      built = true;
    } else {
      onError();

      int len = 0;
      glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &len);
      std::vector<char> msg(len);
      glGetProgramInfoLog(programID, len, &len, &msg[0]);

      vo_err("PROGRAM {}\n{}", programName, std::string_view(msg.data(), msg.size()));

      return;
    }
  }
};

/// Returns the ID of the uniform associated to that string,
/// if its cached, return from cache, else request it to OpenGL
/// and store on cache.
int Shader::getUniformLocation(char const *name) const {
  if (uniformCache.count(name) > 0) { return uniformCache.at(name); }
  int uniformLocation = glGetUniformLocation(programID, name);

  if (uniformLocation > -1) {
    uniformCache.emplace(name, uniformLocation);
  } else if (!uniformAlertCache[name]) {
    vo_log("({}) Not found/used uniform: {}", programName, name);
    uniformAlertCache[name] = true;
  }

  return uniformLocation;
}

/// SET UNIFORMS
void Shader::setUniformMat4(char const *name, glm::mat4 const &mat) const {
  glProgramUniformMatrix4fv(programID, getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setUniformFloat1(char const *name, float f) const {
  glProgramUniform1f(programID, getUniformLocation(name), f);
}
void Shader::setUniformFloat3(char const *name, float f1, float f2, float f3) const {
  glProgramUniform3f(programID, getUniformLocation(name), f1, f2, f3);
}
void Shader::setUniformFloat3(char const *name, glm::vec3 const &floats) const {
  glProgramUniform3fv(programID, getUniformLocation(name), 1, glm::value_ptr(floats));
}
void Shader::setUniformInt1(char const *name, int i) const {
  glProgramUniform1i(programID, getUniformLocation(name), i);
}

/// Connect a given UBO
void Shader::setUniformBlock(char const *name, int uboBindPoint) const {
  int ubShaderIdx = 0;

  if (uniformBlockCache.count(name) > 0) {
    ubShaderIdx = uniformBlockCache.at(name);

  } else {
    ubShaderIdx = glGetUniformBlockIndex(programID, name);

    if (ubShaderIdx > -1) {
      uniformBlockCache.emplace(name, ubShaderIdx);

    } else if (!uniformBlockAlertCache[name]) {
      vo_log("({}) Not found uniform_block: {}", programName, name);
      uniformBlockAlertCache[name] = true;
      return;
    }
  }
  glUniformBlockBinding(programID, ubShaderIdx, uboBindPoint);
}

} // namespace Vonsai
