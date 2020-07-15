#version 410 core

out vec4 f_color;
uniform vec3 u_normalColor;

void main() {
    f_color = vec4(u_normalColor,1);
}
