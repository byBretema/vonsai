#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float idx;

out vec2 v_texCoord;
flat out uint v_idx;

uniform mat4 u_model[3];

layout(std140) uniform camera {
  mat4 u_view;
  mat4 u_proj;
};

///////////////////////////////////////////////////////////////////////////////

void main() {
	v_texCoord = texCoord;
	v_idx = uint(idx);

	gl_Position = u_proj * u_view * u_model[v_idx] * vec4(pos, 1);
}
