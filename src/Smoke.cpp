//
// Created by 任含菲 on 2019-05-07.
//

#include "Smoke.h"

Smoke::Smoke(Vector3D startPos, Vector3D velocity, float density, float particle_size) : density(density),
                                                                                         particle_size(particle_size) {
  status = LIVING;
  particles.resize((int) density);

  for (SmokeParticle &p: particles) {
    // initialize smoke particles
    p.position = startPos;
    p.velocity = velocity / 5.;
  }

}

Smoke::~Smoke() {}

void Smoke::simulate(double frames_per_sec, double simulation_steps) {

  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  if (status == DEAD)
    return;

  int numDied = 0;
  if (status == LIVING) {
    for (SmokeParticle &p: particles) {
      p.lifetime -= delta_t;
      if (p.lifetime < EPS_F) {
        numDied++;
        if (numDied > 0.2 * particles.size()) {
          status = DEAD;
          continue;
        }
        continue;
      }

      // update smoke particles
      Vector3D acc = rand_acc();
      p.position += p.velocity * delta_t;
      p.velocity += acc * delta_t;
      p.position += acc * pow(delta_t, 2);
    }
  }
}


Vector3D Smoke::rand_acc() {
  float accX, accY, accZ;

  srand(time(nullptr));
  accX = (((float) std::rand()) / RAND_MAX - 0.5) * coefX;
  accY = (((float) std::rand()) / RAND_MAX - 0.5) * coefY;
  accZ = (((float) std::rand()) / RAND_MAX - 0.5) * coefX;

  return Vector3D(accX, accY, accZ);
}