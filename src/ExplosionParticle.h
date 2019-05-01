//
// Created by 任含菲 on 2019-04-30.
//

#ifndef CLOTHSIM_EXPLOSIONPARTICLE_H
#define CLOTHSIM_EXPLOSIONPARTICLE_H

#include "CGL/CGL.h"
#include "CGL/vector3D.h"

using namespace CGL;

class ExplosionParticle {

  Vector3D pos;    // position
  Vector3D v;    // velocity
  Vector3D f;    // forces

  float mass;
  float cm;    // thermal mass
  float H;    // heat energy
  float lifetime = 1.0;    // life time
  float r;    // radius

  const float drag = 1.0;    // drag coefficient
  const float conductivity = 1.0;    // thermal conductivity

};


#endif //CLOTHSIM_EXPLOSIONPARTICLE_H
