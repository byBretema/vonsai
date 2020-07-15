#version 410 core

in vec2 v_texCoord;
out vec4 f_color;

layout(std140) uniform globalData {
  mat4 u_view;
  mat4 u_proj;
};

uniform sampler2D u_texture;

void main() {
    f_color = texture(u_texture, v_texCoord);
}
