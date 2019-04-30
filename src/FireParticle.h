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
    FireParticle(Vector3D position = Vector3D(0,0,0), Vector3D velocity = Vector3D(0,1,0), float mass = 1.0)
            : start_position(position), position(position), velocity(velocity), mass(mass){}

    // static values
    Vector3D start_position;
    float mass;
    // dynamic values
    Vector3D position;
    Vector3D forces;
    float lifetime = 1.0;
    Vector3D velocity;
    float alpha = 1.0;
};

#endif //CLOTHSIM_FIREPARTICLE_H
