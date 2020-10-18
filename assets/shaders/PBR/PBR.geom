#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_pos[];
in vec3 v_normal[];
in vec2 v_texCoord[];

out vec3 g_pos;
out vec3 g_normal;
out vec2 g_texCoord;

layout(std140) uniform camera {
  mat4 u_proj;
  mat4 u_view;
};
uniform mat4 u_modelView;
uniform mat4 u_normalMat;

void main()
{
    for (int i=0; i < 3; i++) {
        g_pos = v_pos[i];
        g_normal = v_normal[i];
        g_texCoord = v_texCoord[i];
        gl_Position = u_proj * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
