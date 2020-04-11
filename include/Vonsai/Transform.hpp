#pragma once
#include "Wraps/_glm.hpp"

namespace Vonsai {

class Transform {
public:
  glm::mat4                                   matrix() const;
  std::tuple<glm::vec3, glm::vec3, glm::vec3> getVectors() const;

  // TRANSLATION

  inline float            getPosX() { return m_pos.x; }
  inline float            getPosY() { return m_pos.y; }
  inline float            getPosZ() { return m_pos.z; }
  inline glm::vec3 const &getPos() const { return m_pos; }

  inline void setPosX(float a_x) { m_pos.x = a_x; }
  inline void setPosY(float a_y) { m_pos.y = a_y; }
  inline void setPosZ(float a_z) { m_pos.z = a_z; }
  inline void setPos(glm::vec3 const &a_pos) { m_pos = a_pos; }

  inline void modPosX(float a_x) { m_pos.x += a_x; }
  inline void modPosY(float a_y) { m_pos.y += a_y; }
  inline void modPosZ(float a_z) { m_pos.z += a_z; }
  inline void modPos(glm::vec3 const &a_pos) { m_pos += a_pos; }

  // ROTATION

  inline float            getRotX() { return m_rot.x; }
  inline float            getRotY() { return m_rot.y; }
  inline float            getRotZ() { return m_rot.z; }
  inline glm::vec3 const &getRot() const { return m_rot; }

  /* clang-format off */
  inline void setRotX(float a_x) { m_rot.x = a_x; applyRotLimits(); }
  inline void setRotY(float a_y) { m_rot.y = a_y; applyRotLimits(); }
  inline void setRotZ(float a_z) { m_rot.z = a_z; applyRotLimits(); }
  inline void setRot(glm::vec3 const &a_rot) { m_rot = a_rot; applyRotLimits(); }

  inline void modRotX(float a_x) { m_rot.x += a_x; applyRotLimits(); }
  inline void modRotY(float a_y) { m_rot.y += a_y; applyRotLimits(); }
  inline void modRotZ(float a_z) { m_rot.z += a_z; applyRotLimits(); }
  inline void modRot(glm::vec3 const &a_rot) { m_rot += a_rot; applyRotLimits(); }
  /* clang-format on */

  // SCALE

  inline float            getSclX() { return m_scl.x; }
  inline float            getSclY() { return m_scl.y; }
  inline float            getSclZ() { return m_scl.z; }
  inline glm::vec3 const &getScl() const { return m_scl; }

  inline void setSclX(float a_x) { m_scl.x = a_x; }
  inline void setSclY(float a_y) { m_scl.y = a_y; }
  inline void setSclZ(float a_z) { m_scl.z = a_z; }
  inline void setScl(glm::vec3 const &a_scl) { m_scl = a_scl; }

  inline void modSclX(float a_x) { m_scl.x += a_x; }
  inline void modSclY(float a_y) { m_scl.y += a_y; }
  inline void modSclZ(float a_z) { m_scl.z += a_z; }
  inline void modScl(glm::vec3 const &a_scl) { m_scl += a_scl; }

  void reset();

private:
  glm::vec3 m_pos{0.f};
  glm::vec3 m_rot{0.f};
  glm::vec3 m_scl{1.f};

  void applyRotLimits();
  void info();
};

} // namespace Vonsai
