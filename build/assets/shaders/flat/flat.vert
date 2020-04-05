#version 410 core

layout(location = 0) in vec3 pos;

layout(std140) uniform globalData {
  mat4 u_proj;
  mat4 u_view;
};
uniform mat4 u_modelView;

void main() {
	gl_Position = u_proj * u_modelView * vec4(pos, 1);
}
