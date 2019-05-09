//
// Created by EBao on 2019-05-09.
//

#ifndef CLOTHSIM_SKYBOX_H
#define CLOTHSIM_SKYBOX_H

#include <nanogui/nanogui.h>

//#include "../ray.h"
//#include "../clothMesh.h"
#include "collisionObject.h"
#include "intersection.h"

using namespace nanogui;
using namespace CGL;
using namespace std;

class SkyBox {
public:
    SkyBox(Vector3D center, float dist);
    void render(GLShader& shader);

private:
    Vector3f faces[6][4];
};


#endif //CLOTHSIM_SKYBOX_H
