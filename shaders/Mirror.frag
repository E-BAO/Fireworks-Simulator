#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform samplerCube u_texture_cubemap;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;

out vec4 out_color;

void main() {
  // YOUR CODE HERE

	vec3 camVec = u_cam_pos - v_position.xyz;
	vec3 wo = normalize(camVec);
	vec3 axis = 2 * dot(v_normal.xyz, wo.xyz) * v_normal.xyz;
	vec3 wi = axis - wo;
	vec4 u_color  = texture(u_texture_cubemap, wo);


	vec3 lightVec = u_light_pos - v_position.xyz;

	vec3 lightDir = normalize(lightVec);

  	vec3 n = normalize(v_normal.xyz);
	
	float r2 = 1.0 / dot(lightVec, lightVec);

  	float diffuseDot = dot(n, lightDir);

	float kd = 0.5;

	vec3 diffuse = u_light_intensity * r2 * kd * max(diffuseDot, 0.0);

  	vec3 Ia = u_color.xyz;// vec3(1,1,1);
	float ka = 0.6;

	vec3 ambient = ka * Ia;

	float ks = 1.8;
	float p = 16.0;
	vec3 camDir = normalize(camVec);
	vec3 halfVec = normalize(camDir + lightDir);
  	float specularDot = dot(n, halfVec);
	vec3 specular = u_light_intensity * r2 * ks * pow(max(specularDot, 0.0),p);

	out_color = vec4(0,0.5,0.5,1);//u_color * vec4(diffuse + ambient + specular, 1);

}
