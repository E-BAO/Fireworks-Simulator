#version 330

in vec4 v_color;

out vec4 out_color;

void main() {

    out_color = v_color;

    float dist = length(gl_PointCoord - vec2(0.5));
    if(dist > 0.5){
        discard;
    }else{
//        out_color += vec4(1.0,1.0,1.0,0.0) * (0.5 - dist);
        out_color.a *= 2.0 * (0.5 - dist);
    }
}

