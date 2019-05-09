#version 330

// Color
in vec4 v_color;

// Properties of the single point light
uniform vec3 u_light_pos[10];
uniform int u_light_num;
uniform vec3 u_light_intensity[10];
uniform float u_kd[10];
uniform sampler2D u_texture_1;

// These are the inputs which are the outputs of the vertex shader.
in vec4 v_position;
in vec4 v_normal;

// This is where the final pixel color is output.
// Here, we are only interested in the first 3 dimensions (xyz).
// The 4th entry in this vector is for "alpha blending" which we
// do not require you to know about. For now, just set the alpha
// to 1.
out vec4 out_color;

void main() {
    vec3 n = normalize(v_normal.xyz);
    float ka = 0.3;
    vec3 Ia = v_color.xyz;
    vec3 ambient = ka * Ia;
    vec3 diffuse = vec3(0,0,0);

    for(int i = 0; i < u_light_num; i ++){
        vec3 lightVec = u_light_pos[i] - v_position.xyz;

        vec3 lightDir = normalize(lightVec);

        float diffuseDot = dot(n, lightDir);


        diffuse += u_light_intensity[i] * max(u_kd[i],0) * max(diffuseDot, 0.0);
    }

    out_color = v_color * vec4(ambient + diffuse, 1);
}