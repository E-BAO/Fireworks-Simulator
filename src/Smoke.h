//
// Created by 任含菲 on 2019-05-07.
//

#ifndef CLOTHSIM_SMOKE_H
#define CLOTHSIM_SMOKE_H

#include "CGL/CGL.h"
#include "CGL/misc.h"

#include "Smoke.h"
#include "SmokeParticle.h"
#include <nanogui/nanogui.h>

using namespace CGL;
using namespace std;

enum SmokeStatus { LIVING = 0, DEAD = 1 };

class Smoke {

public:
  Smoke(Vector3D startPos = Vector3D(0,0,0), Vector3D velocity = Vector3D(0,0,0), float density = 50.0,
  float particle_size = 50.0, float range = 0.7);
  ~Smoke();

  void simulate(double frames_per_sec, double simulation_steps);

  SmokeStatus status;
  vector<SmokeParticle> particles;
  float particle_size;
  nanogui::Color color = nanogui::Color(0.7f, 0.7f, 0.7f, 0.15f);

private:
  Vector3D rand_pos();
  Vector3D rand_acc();

  float density;
  float range;
  float coefX = 100;
  float coefY = 25;
  float damping = 0.999;

};


#endif //CLOTHSIM_SMOKE_H
