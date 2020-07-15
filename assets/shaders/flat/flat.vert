#version 410 core

layout(location = 0) in vec3 pos;

uniform mat4 u_modelView;

layout(std140) uniform camera {
  mat4 u_proj;
  mat4 u_view;
};

///////////////////////////////////////////////////////////////////////////////

void main() {
	gl_Position = u_proj * u_modelView * vec4(pos, 1);
}
