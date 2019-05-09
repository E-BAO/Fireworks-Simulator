#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;
uniform vec4 u_color;

in vec4 in_position;
in float in_particle_size;

out vec4 v_color;

void main() {
    gl_Position = u_view_projection * u_model * in_position;

    gl_PointSize = in_particle_size;// / gl_Position.z;

    v_color = u_color;
}
