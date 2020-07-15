#version 410 core

layout (triangles) in;
layout (line_strip, max_vertices = 3) out;

in vec3 v_normal[];

layout(std140) uniform camera {
  mat4 u_proj;
};

uniform float u_normalSize=0.25;

void main()
{
    for (int i=0; i < 3; i++) {
        gl_Position = u_proj * gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = u_proj * (gl_in[i].gl_Position + vec4(v_normal[i],0) * u_normalSize);
        EmitVertex();
        EndPrimitive();
    }
}
