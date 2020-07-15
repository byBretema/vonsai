#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 v_normal;

layout(std140) uniform camera {
  mat4 u_proj;
};
uniform mat4 u_modelView;
uniform mat4 u_normalMat;

void main()
{
	v_normal = normal;
	// v_normal = normalize(u_normalMat * vec4(normalize(normal),0)).xyz;
	gl_Position = u_modelView * vec4(pos, 1);
}
