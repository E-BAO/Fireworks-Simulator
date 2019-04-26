//
// Created by EBao on 2019-04-26.
//

#ifndef CLOTHSIM_FIREPARTICLE_H
#define CLOTHSIM_FIREPARTICLE_H


#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "CGL/vector3D.h"

using namespace CGL;

struct FireParticle {
    FireParticle(Vector3D position)
            : start_position(position), position(position){}

    // static values
    Vector3D start_position;

    // dynamic values
    Vector3D position;
    Vector3D last_position;
    Vector3D forces;
    float lifetime;
    Vector3D velocity;
};

#endif //CLOTHSIM_FIREPARTICLE_H
