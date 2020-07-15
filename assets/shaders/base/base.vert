#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoord;

out vec2 v_texCoord;

layout(std140) uniform globalData {
  mat4 u_view;
  mat4 u_proj;
};
uniform mat4 u_model;

void main() {
	v_texCoord = texCoord;
	gl_Position = u_proj * u_view * u_model * vec4(pos, 1);
}
