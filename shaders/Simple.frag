#version 330

in vec4 v_color;

out vec4 out_color;

void main() {
    out_color = v_color;

    vec2 vv = gl_PointCoord;
    if (length(gl_PointCoord - vec2(0.5)) > 0.5){
        discard;
    }
}

