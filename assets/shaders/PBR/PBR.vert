#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_texCoord;

// uniform vec4 u_ColorDiffuse;
// uniform vec4 u_ColorSpecular;
// uniform vec4 u_ColorAmbient;
// uniform vec4 u_ColorEmissive;
// uniform float u_Shine;
// uniform float u_ShineIntensity;
// uniform float u_Reflectivity;
// uniform float u_Refraction;

// uniform sampler2D u_TexDiffuse;
// uniform sampler2D u_TexSpecular;
// uniform sampler2D u_TexAmbient;
// uniform sampler2D u_TexEmissive;
// uniform sampler2D u_TexHeight;
// uniform sampler2D u_TexShininess;
// uniform sampler2D u_TexOpacity;
// uniform sampler2D u_TexDisplacement;
// uniform sampler2D u_TexLightmap;
// uniform sampler2D u_TexReflection;

layout(std140) uniform camera {
  mat4 u_proj;
  mat4 u_view;
};
uniform mat4 u_modelView;
uniform mat4 u_normalMat;

void main() {
	v_texCoord = texCoord;
	v_pos = (u_modelView * vec4(pos, 1)).xyz;
	v_normal = normalize(u_normalMat * vec4(normal,0)).xyz;
	gl_Position = u_modelView * vec4(pos, 1);
}
