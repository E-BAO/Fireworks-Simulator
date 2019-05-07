//
// Created by 任含菲 on 2019-05-07.
//

#ifndef CLOTHSIM_SMOKEPARTICLE_H
#define CLOTHSIM_SMOKEPARTICLE_H

#include "CGL/CGL.h"
#include "CGL/vector3D.h"

using namespace CGL;

class SmokeParticle {
public:
  SmokeParticle(Vector3D position = Vector3D(0, 0, 0), Vector3D velocity = Vector3D(0, 0, 0))
      : start_position(position), position(position), velocity(velocity) {}

  void update();

  // static values
  Vector3D start_position;
  // dynamic values
  Vector3D position;
  Vector3D velocity;
  float lifetime = 5.0;
};


#endif //CLOTHSIM_SMOKEPARTICLE_H
