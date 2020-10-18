#include <Vonsai/Shader.hpp>

#include <Vonsai/Utils/Files.hpp>
#include <Vonsai/Utils/Logger.hpp>

#include <Vonsai/Wraps/_gl.hpp>

#include <utility>
#include <vector>

namespace Vonsai {

std::array<const char *, 5> constexpr SHADER_NAMES{
    "VERTEX",       //
    "TESS_CONTROL", //
    "TESS_EVAL",    //
    "GEOMETRY",     //
    "FRAGMENT"      //
};
// ---------------------------------------------------------------------------

std::array<int, 5> constexpr SHADER_TYPES{
    GL_VERTEX_SHADER,          //
    GL_TESS_CONTROL_SHADER,    //
    GL_TESS_EVALUATION_SHADER, //
    GL_GEOMETRY_SHADER,        //
    GL_FRAGMENT_SHADER         //
};

// ---------------------------------------------------------------------------

Shader::Shader(std::string const &a_name) : m_programName(a_name) {
  m_uniformCache.reserve(128);
  m_uniformAlertCache.reserve(128);
  m_uniformBlockCache.reserve(128);
  m_uniformBlockAlertCache.reserve(128);
}

// ---------------------------------------------------------------------------

Shader::Shader(std::string const &a_name, ShaderPath const &a_paths) : Shader(a_name) {
  ShaderCode rawCode;
  for (auto i = 0u; i < SHADER_TYPES.size(); ++i) { rawCode[i] = Files::fromString(a_paths[i]); }
  buildPipeline(rawCode);
}

// ---------------------------------------------------------------------------

Shader::Shader(std::string const &a_name, ShaderCode const &a_rawCode) : Shader(a_name) { buildPipeline(a_rawCode); }

// ---------------------------------------------------------------------------

Shader::Shader(Shader &&rhs) noexcept { swap(*this, rhs); }

// ---------------------------------------------------------------------------

Shader &Shader::operator=(Shader &&rhs) noexcept {
  swap(*this, rhs);
  return *this;
}

// ---------------------------------------------------------------------------

void swap(Shader &lhs, Shader &rhs) noexcept {
  using std::swap;
  swap(lhs.m_programID, rhs.m_programID);
  swap(lhs.m_programName, rhs.m_programName);
  swap(lhs.m_valid, rhs.m_valid);
  swap(lhs.m_built, rhs.m_built);
  swap(lhs.m_uniformCache, rhs.m_uniformCache);
  swap(lhs.m_uniformAlertCache, rhs.m_uniformAlertCache);
  swap(lhs.m_uniformBlockCache, rhs.m_uniformBlockCache);
  swap(lhs.m_uniformBlockAlertCache, rhs.m_uniformBlockAlertCache);
}

// ---------------------------------------------------------------------------

bool Shader::isReady() const { return m_valid && m_built; }

// ---------------------------------------------------------------------------

int Shader::getUniformLocation(std::string const &a_name) const {
  auto [kv, ins]      = m_uniformCache.try_emplace(a_name, glGetUniformLocation(m_programID, a_name.c_str()));
  int uniformLocation = kv->second;

  if (ins and uniformLocation < 0) {
    vo_log("({}) Not found/used uniform: {}", m_programName, a_name);
    m_uniformAlertCache[a_name] = true;
  }

  return uniformLocation;
}

// ---------------------------------------------------------------------------

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

void Shader::setFloat4(std::string const &a_name, glm::vec4 const &a_floats) const {
  glProgramUniform3fv(m_programID, getUniformLocation(a_name), 1, glm::value_ptr(a_floats));
}

void Shader::setMat4(std::string const &a_name, glm::mat4 const &a_mat) const {
  glProgramUniformMatrix4fv(m_programID, getUniformLocation(a_name), 1, GL_FALSE, glm::value_ptr(a_mat));
}

// ---------------------------------------------------------------------------

// Connect a given UBO
void Shader::linkUBO(std::string const &a_name, unsigned int a_uboBindPoint) const {
  // glUniformBlockBinding(m_programID, glGetUniformBlockIndex(m_programID, a_name.c_str()), a_uboBindPoint);
  // return;

  // vo_log("({}) Linking UBO '{}' at idx {}", m_programName, a_name, a_uboBindPoint);

  auto [kv, ins] = m_uniformBlockCache.try_emplace(a_name, glGetUniformBlockIndex(m_programID, a_name.c_str()));
  if (!ins) return;

  if (int ubShaderIdx = kv->second; ubShaderIdx >= 0)
    glUniformBlockBinding(m_programID, ubShaderIdx, a_uboBindPoint);
  else if (!m_uniformBlockAlertCache[a_name]) {
    vo_log("({}) Not found UBO '{}'", m_programName, a_name);
    m_uniformBlockAlertCache[a_name] = true;
  }
}

// ---------------------------------------------------------------------------

void Shader::bind() const { GL_ASSERT(glUseProgram(m_programID)); }
void Shader::unbind() const { GL_ASSERT(glUseProgram(0)); }

// ---------------------------------------------------------------------------

// Build pipeline from code as string

using ErrFn = std::function<void(std::string const &)>;

int buildShader(unsigned int type, const char *src, int srcSize, ErrFn const &onErr, std::string const &errFmt) {
  if (srcSize < 1) return -1;

  unsigned int const sID = glCreateShader(type);
  GL_ASSERT(glShaderSource(sID, 1, &src, &srcSize));
  GL_ASSERT(glCompileShader(sID));

  GLint compiled;
  glGetShaderiv(sID, GL_COMPILE_STATUS, &compiled);
  if (compiled) { return sID; }

  int len;
  glGetShaderiv(sID, GL_INFO_LOG_LENGTH, &len);
  std::vector<char> msg(len);
  glGetShaderInfoLog(sID, len, &len, &msg[0]);
  onErr(errFmt + std::string(msg.data(), msg.size()));

  glDeleteShader(sID);
  return -1;
}

// ---

bool linkProgram(unsigned int pID, ErrFn const &onErr, std::string const &errFmt) {
  GL_ASSERT(glLinkProgram(pID));

  int linked;
  glGetProgramiv(pID, GL_LINK_STATUS, &linked);
  if (linked) { return true; }

  int len = 0;
  glGetProgramiv(pID, GL_INFO_LOG_LENGTH, &len);
  std::vector<char> msg(len);
  glGetProgramInfoLog(pID, len, &len, &msg[0]);
  onErr(errFmt + std::string(msg.data(), msg.size()));

  return false;
}

// ---

void Shader::buildPipeline(ShaderCode const &a_rawCode) {
  if (m_built) { assert(0); };

  // Triggered function on error cases
  auto onErr = [&](std::string const &errMsg = "") {
    glDeleteProgram(m_programID);
    m_programID = 0;
    m_valid     = false;
    vo_err(errMsg);
  };

  // Request program to OpenGL
  m_programID = glCreateProgram();

  // Append shaders
  for (auto i = 0u; i < SHADER_TYPES.size(); ++i) {
    auto const errFmt   = vo_fmt("SHADER ({}) - {}\n", SHADER_NAMES[i], m_programName);
    int const  shaderID = buildShader(SHADER_TYPES[i], a_rawCode[i].c_str(), a_rawCode[i].size(), onErr, errFmt);
    if (shaderID > -1) {
      glAttachShader(m_programID, shaderID);
      glDeleteShader(shaderID);
    }
  }

  // Link program to the current context
  if (linkProgram(m_programID, onErr, vo_fmt("PROGRAM {}\n", m_programName))) { m_built = true; }
}

// ---------------------------------------------------------------------------

} // namespace Vonsai
