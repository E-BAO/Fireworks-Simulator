#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  vec4 color = texture(u_texture_2, uv);
  return color.r;
}

void main() {
  // YOUR CODE HERE
  
  vec3 b = cross(v_normal.xyz,v_tangent.xyz);
  float factor = u_height_scaling * u_normal_scaling;
  float huv = h(v_uv);
  float dU = (h(vec2(v_uv.x + 1.0 / u_texture_2_size.x, v_uv.y)) - huv) * factor;
  float dV = (h(vec2(v_uv.x, v_uv.y + 1.0 / u_texture_2_size.y)) - huv) * factor;
  vec3 n0 = vec3(-dU, -dV, 1.0);
  mat3 tbn = mat3(v_tangent.xyz, b, v_normal.xyz);
  vec3 nd = tbn * n0;

	vec3 lightVec = u_light_pos - v_position.xyz;

	vec3 lightDir = normalize(lightVec);

  	vec3 n = normalize(nd);
	
	float r2 = 1.0 / dot(lightVec, lightVec);

  	float diffuseDot = dot(n, lightDir);

	float kd = 0.5;

	vec3 diffuse = u_light_intensity * r2 * kd * max(diffuseDot, 0.0);

  	vec3 Ia = u_color.xyz;// vec3(1,1,1);
	float ka = 0.6;

	vec3 ambient = ka * Ia;

	float ks = 1.8;
	float p = 16.0;
	vec3 camVec = u_cam_pos - v_position.xyz;
	vec3 camDir = normalize(camVec);
	vec3 halfVec = normalize(camDir + lightDir);
  	float specularDot = dot(n, halfVec);
	vec3 specular = u_light_intensity * r2 * ks * pow(max(specularDot, 0.0),p);

	out_color = vec4(0,0,1,1);//u_color * vec4(diffuse + ambient + specular, 1);
}

