#version 330

// Properties of the single point light
uniform vec3 u_light_pos[10];
uniform int u_light_num;
uniform vec3 u_light_intensity[10];
uniform float u_kd[10];
uniform sampler2D u_texture_1;

// These are the inputs which are the outputs of the vertex shader.
in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

// This is where the final pixel color is output.
// Here, we are only interested in the first 3 dimensions (xyz).
// The 4th entry in this vector is for "alpha blending" which we
// do not require you to know about. For now, just set the alpha
// to 1.
out vec4 out_color;

void main() {
    vec4 t_color = texture(u_texture_1,v_uv);// * vec4(ambient + diffuse, 1);

    vec3 n = normalize(v_normal.xyz);
    float ka = 1.0;
    vec3 Ia = vec3(1,1,1);
    vec3 ambient = ka * Ia;
    vec3 diffuse = vec3(0,0,0);

    for(int i = 0; i < u_light_num; i ++){
        vec3 lightVec = u_light_pos[i] - v_position.xyz;

        vec3 lightDir = normalize(lightVec);

        float diffuseDot = dot(n, lightDir);


        diffuse += u_light_intensity[i] * max(u_kd[i],0) * max(diffuseDot, 0.0);
    }

    out_color = t_color * vec4(ambient + diffuse * 2.0, 1);
}