#version 410 core

//=====================================================================================================================

out vec4 f_color;

in vec3 g_pos;
in vec3 g_normal;
in vec2 g_texCoord;

uniform vec4 u_ColorDiffuse;
uniform vec4 u_ColorSpecular;
uniform vec4 u_ColorAmbient;
uniform vec4 u_ColorEmissive;
uniform float u_Shine;
uniform float u_Refraction;

uniform sampler2D u_TexDiffuse;
uniform sampler2D u_TexSpecular;
uniform sampler2D u_TexAmbient;
uniform sampler2D u_TexEmissive;
// uniform sampler2D u_TexHeight;
// uniform sampler2D u_TexShininess;
// uniform sampler2D u_TexOpacity;
// uniform sampler2D u_TexDisplacement;
// uniform sampler2D u_TexLightmap;
// uniform sampler2D u_TexReflection;

//=====================================================================================================================

float Ns; // Roughness
float Ni; // Refraction
vec3 Kd;  // Diffuse Color
vec3 Ka;  // Ambient Color
vec3 Ks;  // Specular Color
vec3 Ke;  // Emissive Color

//=====================================================================================================================

struct Light {
  vec4 pos;
  vec4 color;
  vec3 attenuation;
  float intensity;
};

layout(std140) uniform lights {
  vec4 u_numLights;
  Light u_lights[100];
};

//=====================================================================================================================

layout(std140) uniform camera {
  mat4 u_proj;
  mat4 u_view;
};

//=====================================================================================================================

vec3 shade(in Light light, in vec3 V, in vec3 N, in vec3 C){

	vec3 L = normalize((u_view * light.pos).xyz - g_pos);
	vec3 H = normalize(L + V); // For blinn phong

  // Attenuation
  float lightDist = length(L);
  float att1 = light.attenuation.x;
  float att2 = light.attenuation.y * lightDist;
  float att3 = light.attenuation.y * lightDist * lightDist;
	float attenuation = 1.0 / max((att1+att2+att3), 1.0);

	// Object color
  vec3 diffuse  = Kd * attenuation * clamp(dot(L, N), 0, 1);
  vec3 specular = Ks * pow(clamp(dot(N, H), 0, 1), Ns);

	// Contribution of light to the object
	return C * light.color.rgb * light.intensity * attenuation * (diffuse + specular);
}

//=====================================================================================================================

vec3 texColor(in sampler2D tex, in vec4 altColor) {
  // @DANI : Textures could have black color... set a default negative texture to check if exists
  vec3 color = texture(tex, g_texCoord).rgb;
  if(color.x == 0. && color.y == 0. && color.z == 0.) { color = altColor.rgb; }
  if(color.x == 0. && color.y == 0. && color.z == 0.) { color = vec3(1., 0., 1.); }
  return color;
}

//=====================================================================================================================

void main() {

  Ns = u_Shine;
  Ni = u_Refraction;
  Kd = texColor(u_TexDiffuse , u_ColorDiffuse );
  Ks = texColor(u_TexSpecular, u_ColorSpecular);
  Ka = texColor(u_TexAmbient , u_ColorAmbient );
  Ke = texColor(u_TexEmissive, u_ColorEmissive);

  vec3 finalColor = Kd;

  // vec3 texcolor = texture(u_TexDiffuse, g_texCoord).rgb;
  // if(texcolor.x == 0 && texcolor.y == 0 && texcolor.z == 0){
  //   texcolor = vec3(1,0,1);
  // }

  // for (int i = 0; i < u_numLights.w; i++) {
  //   finalColor += shade(u_lights[i], normalize(-g_pos), normalize(g_normal), texcolor);
  // }

  f_color = vec4(finalColor, 1);
}

//=====================================================================================================================
