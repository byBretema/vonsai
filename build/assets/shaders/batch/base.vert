#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float idx;

out vec2 v_texCoord;
flat out uint v_idx;

layout(std140) uniform globalData {
  mat4 view;
  mat4 proj;
};
uniform mat4 model[3];

void main() {
	v_texCoord = texCoord;
	v_idx = uint(idx);

	gl_Position = proj * view * model[v_idx] * vec4(pos, 1);
}
