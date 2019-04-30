#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;

in vec4 in_position;
in vec4 in_color;

out vec4 v_color;

void main() {
    gl_Position = u_view_projection * u_model * in_position;
    gl_PointSize = 10.0;
    v_color = in_color;
}
