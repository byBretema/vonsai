#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_texCoord;

layout(std140) uniform globalData {
  mat4 u_proj;
  mat4 u_view;
};
uniform mat4 u_modelView;
uniform mat4 u_normalMat;

void main() {
	v_texCoord = texCoord;
	v_pos = (u_modelView * vec4(pos, 1)).xyz;
	v_normal = (u_normalMat * vec4(normal,0)).xyz;
	gl_Position = u_proj * u_modelView * vec4(pos, 1);
}
