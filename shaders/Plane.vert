#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;
uniform vec4 u_color;

in vec4 in_position;
in vec4 in_normal;

out vec4 v_color;
out vec4 v_position;
out vec4 v_normal;

void main() {
    gl_Position = u_view_projection * u_model * in_position;

    v_position = u_model * in_position;
    v_normal = normalize(u_model * in_normal);


    v_color = u_color;
}
