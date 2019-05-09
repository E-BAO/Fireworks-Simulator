//
// Created by EBao on 2019-05-09.
//

#include "SkyBox.h"

SkyBox::SkyBox(Vector3D center, float dist){
    float xmin = center.x - dist;
    float xmax = center.x + dist;
    float ymin = center.y;// - dist;
    float ymax = center.y + 2.0 * dist;
    float zmin = center.z - dist;
    float zmax = center.z + dist;


    // 2_____4
    // |  \
    // 1_____3

    //back
    faces[0][0] = Vector3f(xmin, ymin, zmin);
    faces[0][1] = Vector3f(xmin, ymax, zmin);
    faces[0][2] = Vector3f(xmax, ymin, zmin);
    faces[0][3] = Vector3f(xmax, ymax, zmin);

    //front
    faces[1][0] = Vector3f(xmin, ymin, zmax);
    faces[1][1] = Vector3f(xmin, ymax, zmax);
    faces[1][2] = Vector3f(xmax, ymin, zmax);
    faces[1][3] = Vector3f(xmax, ymax, zmax);

    //left
    faces[2][0] = Vector3f(xmin, ymin, zmax);
    faces[2][1] = Vector3f(xmin, ymax, zmax);
    faces[2][2] = Vector3f(xmin, ymin, zmin);
    faces[2][3] = Vector3f(xmin, ymax, zmin);


    //right
    faces[3][0] = Vector3f(xmax, ymin, zmin);
    faces[3][1] = Vector3f(xmax, ymax, zmin);
    faces[3][2] = Vector3f(xmax, ymin, zmax);
    faces[3][3] = Vector3f(xmax, ymax, zmax);

    //top
    faces[4][0] = Vector3f(xmin, ymax, zmax);
    faces[4][1] = Vector3f(xmin, ymax, zmin);
    faces[4][2] = Vector3f(xmax, ymax, zmax);
    faces[4][3] = Vector3f(xmax, ymax, zmin);

    //bottom
    faces[5][0] = Vector3f(xmin, ymin, zmax);
    faces[5][1] = Vector3f(xmin, ymin, zmin);
    faces[5][2] = Vector3f(xmax, ymin, zmax);
    faces[5][3] = Vector3f(xmax, ymin, zmin);
}


void SkyBox::render(GLShader &shader) {
    int num_tris = 12;
    MatrixXf positions(3, num_tris * 3);
    MatrixXf uvs(2, num_tris * 3);

    positions.col(0) << faces[0][0].x(), faces[0][0].y(), faces[0][0].z();
    positions.col(1) << faces[0][1].x(), faces[0][1].y(), faces[0][1].z();
    positions.col(2) << faces[0][2].x(), faces[0][2].y(), faces[0][2].z();

    positions.col(3) << faces[0][1].x(), faces[0][1].y(), faces[0][1].z();
    positions.col(4) << faces[0][2].x(), faces[0][2].y(), faces[0][2].z();
    positions.col(5) << faces[0][3].x(), faces[0][3].y(), faces[0][3].z();

    positions.col(6) << faces[1][0].x(), faces[1][0].y(), faces[1][0].z();
    positions.col(7) << faces[1][1].x(), faces[1][1].y(), faces[1][1].z();
    positions.col(8) << faces[1][2].x(), faces[1][2].y(), faces[1][2].z();

    positions.col(9) << faces[1][1].x(), faces[1][1].y(), faces[1][1].z();
    positions.col(10) << faces[1][2].x(), faces[1][2].y(), faces[1][2].z();
    positions.col(11) << faces[1][3].x(), faces[1][3].y(), faces[1][3].z();

    positions.col(12) << faces[2][0].x(), faces[2][0].y(), faces[2][0].z();
    positions.col(13) << faces[2][1].x(), faces[2][1].y(), faces[2][1].z();
    positions.col(14) << faces[2][2].x(), faces[2][2].y(), faces[2][2].z();

    positions.col(15) << faces[2][1].x(), faces[2][1].y(), faces[2][1].z();
    positions.col(16) << faces[2][2].x(), faces[2][2].y(), faces[2][2].z();
    positions.col(17) << faces[2][3].x(), faces[2][3].y(), faces[2][3].z();

    positions.col(18) << faces[3][0].x(), faces[3][0].y(), faces[3][0].z();
    positions.col(19) << faces[3][1].x(), faces[3][1].y(), faces[3][1].z();
    positions.col(20) << faces[3][2].x(), faces[3][2].y(), faces[3][2].z();

    positions.col(21) << faces[3][1].x(), faces[3][1].y(), faces[3][1].z();
    positions.col(22) << faces[3][2].x(), faces[3][2].y(), faces[3][2].z();
    positions.col(23) << faces[3][3].x(), faces[3][3].y(), faces[3][3].z();

    positions.col(24) << faces[4][0].x(), faces[4][0].y(), faces[4][0].z();
    positions.col(25) << faces[4][1].x(), faces[4][1].y(), faces[4][1].z();
    positions.col(26) << faces[4][2].x(), faces[4][2].y(), faces[4][2].z();

    positions.col(27) << faces[4][1].x(), faces[4][1].y(), faces[4][1].z();
    positions.col(28) << faces[4][2].x(), faces[4][2].y(), faces[4][2].z();
    positions.col(29) << faces[4][3].x(), faces[4][3].y(), faces[4][3].z();

    positions.col(30) << faces[5][0].x(), faces[5][0].y(), faces[5][0].z();
    positions.col(31) << faces[5][1].x(), faces[5][1].y(), faces[5][1].z();
    positions.col(32) << faces[5][2].x(), faces[5][2].y(), faces[5][2].z();

    positions.col(33) << faces[5][1].x(), faces[5][1].y(), faces[5][1].z();
    positions.col(34) << faces[5][2].x(), faces[5][2].y(), faces[5][2].z();
    positions.col(35) << faces[5][3].x(), faces[5][3].y(), faces[5][3].z();


    //back
    uvs.col(0) << 1.0,0.66;
    uvs.col(1) << 1.0,0.34;
    uvs.col(2) << 0.75,0.66;

    uvs.col(3) << 1.0,0.34;
    uvs.col(4) << 0.75,0.66;
    uvs.col(5) << 0.75,0.34;

    //front
    uvs.col(6) << 0.25,0.66;
    uvs.col(7) << 0.25,0.34;
    uvs.col(8) << 0.5,0.66;
    uvs.col(9) << 0.25,0.34;
    uvs.col(10) << 0.5,0.66;
    uvs.col(11) << 0.5,0.34;

    //left
    uvs.col(12) << 0.25,0.66;
    uvs.col(13) << 0.25,0.34;
    uvs.col(14) << 0.0,0.66;

    uvs.col(15) << 0.25,0.34;
    uvs.col(16) << 0.0,0.66;
    uvs.col(17) << 0.0,0.34;

    //right
    uvs.col(18) << 0.75,0.66;
    uvs.col(19) << 0.75,0.34;
    uvs.col(20) << 0.5,0.66;

    uvs.col(21) << 0.75,0.34;
    uvs.col(22) << 0.5,0.66;
    uvs.col(23) << 0.5,0.34;

    //top
    uvs.col(24) << 0.25,0.34;
    uvs.col(25) << 0.25,0.0;
    uvs.col(26) << 0.50,0.34;

    uvs.col(27) << 0.25,0.0;
    uvs.col(28) << 0.50,0.34;
    uvs.col(29) << 0.50,0.0;

    //bottom
    uvs.col(30) << 0.25,0.67;
    uvs.col(31) << 0.25,1.0;
    uvs.col(32) << 0.5,0.67;

    uvs.col(33) << 0.25,1.0;
    uvs.col(34) << 0.5,0.67;
    uvs.col(35) << 0.5,1.0;


    shader.uploadAttrib("in_position", positions);
    shader.uploadAttrib("in_uv", uvs, false);

    shader.drawArray(GL_TRIANGLES, 0, 12 * 3);
}

