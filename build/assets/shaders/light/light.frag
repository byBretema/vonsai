#version 410 core

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_texCoord;

out vec4 f_color;

layout(std140) uniform globalData {
  mat4 u_proj;
  mat4 u_view;
};

struct Light {
  vec4 position;
  vec4 color;
  vec3 attenuation;
  float     intensity;
};
layout(std140) uniform lightsUBO {
  vec4 u_numLights;
  Light u_lights[100];
};

uniform sampler2D u_texture;


  // TODO: Get as uniforms /////////////////////////////////////////////////////////////////////////
  const float ambientI = 0.05;
  const float Ka       = 1.0;

  const float Kd       = 1.0;

  const float Ks       = 1.0;
  const float glossy   = 50.0;
  //////////////////////////////////////////////////////////////////////////////////////////////////

vec3 shade(in Light a_light, in vec3 a_V, in vec3 a_normal, in vec3 a_meshColor){

	vec3  lightVec = normalize((u_view * a_light.position).xyz - v_pos);
	vec3  halfVec = normalize(lightVec + a_V); // For blinn phong

  // Attenuation
  float lightDist = length(lightVec);
  float att1 = a_light.attenuation.x;
  float att2 = a_light.attenuation.y * lightDist;
  float att3 = a_light.attenuation.y * lightDist * lightDist;
	float attenuation = 1.0 / max((att1+att2+att3), 1.0);

	// Object color
  float diffuse  = Kd * attenuation * clamp(dot(lightVec, a_normal), 0, 1);
  float specular = Ks * pow(clamp(dot(a_normal, halfVec), 0, 1), glossy);

	// Contribution of light to the object
	return a_meshColor * a_light.color.rgb * a_light.intensity * attenuation * (diffuse + specular);
}

void main() {

  vec3 finalColor = vec3(ambientI * Ka);

  for (int i = 0; i < 2; i++) {
    finalColor += shade(u_lights[i], normalize(-v_pos), normalize(v_normal), texture(u_texture, v_texCoord).rgb );
  }

  f_color = vec4(finalColor, 1);
}
