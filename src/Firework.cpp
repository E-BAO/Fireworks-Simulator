//
// Created by EBao on 2019-04-26.
//

#include "Firework.h"

Firework::Firework(Vector3D startPos, Vector3D velocity, float density, float energy, float damping,
                   float particle_size, bool blink, bool trail, FireworkShape shape) : startVelocity(velocity),
                                                                                       density(density),
                                                                                       energy(energy), damping(damping),
                                                                                       particle_size(particle_size),
                                                                                       blink(blink), trail(trail),
                                                                                       shape(shape) {
  status = FireworkStatus::IGNITING;
  igniteParticle = new FireParticle(startPos, velocity);
  trailLen *= particle_size;
  if (shape == SPARKLER)
    startVelocity /= 2;
  std::cout << "new fireworks here " << std::endl;
}


Firework::~Firework() {}

void Firework::simulate(double frames_per_sec, double simulation_steps, vector<Vector3D> external_accelerations) {
//    std::cout<< " firework simulate "<< std::endl;

  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  int initStep = (frames_per_sec * simulation_steps / 20);
  if(shape == SPARKLER and totalSteps%initStep == 0 and totalSteps < initStep * 10) {
    initExplosion(false);
  }

  if (status == DIED)
    return;
  if (status == IGNITING) {
    Vector3D lastPos = igniteParticle->position;
    Vector3D lastVec = igniteParticle->velocity;
    igniteParticle->position += igniteParticle->velocity * delta_t;
    for (auto ac: external_accelerations) {
      igniteParticle->position += ac * pow(delta_t, 2);
      igniteParticle->velocity += ac * delta_t;
    }
    if (igniteParticle->velocity.y < EPS_F or
        (shape == SPARKLER and igniteParticle->velocity.y < startVelocity.y * .8)) {
      status = EXPLODING;
      igniteParticle->position = lastPos;
      igniteParticle->velocity = lastVec;
      particles.resize((int) density);
      initExplosion();  //fireworks shape here
    }
  }

  if (status == EXPLODING) {

    if (trail) {
      int particleNum = particles.size();
      for (int i = subNum - 1; i >= 0; --i) {
        for (int j = 0; j < particleNum; ++j) {
          FireParticle &subP = subParticles[i * particleNum + j];
          if (collisionStep > trailLen * i * simulation_steps) {
            subP.position += subP.velocity * delta_t;
            Vector3D dampAc = -(subP.velocity).unit() * (subP.velocity).norm2() * damping;
            subP.velocity += dampAc * delta_t;
            subP.position += dampAc * pow(delta_t, 2);
          }
        }
      }
      collisionStep += 1;
    }

    int numDied = 0;
    for (FireParticle &p: particles) {
      p.lifetime -= delta_t;
      if (p.lifetime < EPS_F) {
        numDied++;
        if (numDied > 0.2 * particles.size()) {
            status = DIED;
            continue;
        }
      }
      p.position += p.velocity * delta_t;
      //add damping here in or out ???
      Vector3D dampAc = -(p.velocity).unit() * (p.velocity).norm2() * damping;
      p.velocity += dampAc * delta_t;
      p.position += dampAc * pow(delta_t, 2);
      //add ac here
      for (auto ac: external_accelerations) {
        p.velocity += ac * delta_t;
        p.position += ac * pow(delta_t, 2);
      }
    }
    totalSteps++;
  }
}

double random_uniform() {
  if (rand() % 2 == 0)
    return ((double) std::rand()) / RAND_MAX;
  else
    return -((double) std::rand()) / RAND_MAX;
}

Vector3D random_uni_velocity() {
  Vector3D v(random_uniform(), random_uniform(), random_uniform());
  return v.unit();
}

void Firework::initParticle(FireParticle *p){
  p->position = igniteParticle->position;
  p->velocity = igniteParticle->velocity;
  Vector3D v_dir = random_uni_velocity();
  // sparkler shape
  if (shape == SPARKLER) {
    p->velocity += 2.0 * startVelocity;
    v_dir = Vector3D(0.2 * v_dir.x, abs(v_dir.y), 0.2 * v_dir.z);
  }

  p->mass = shape == SPHERICAL ? 0.5 : 1.0 + random_uniform();
  p->velocity += v_dir * energy / p->mass;
  p->lifetime = 0.8 + 0.3 * random_uniform();  //change

}

void Firework::initExplosion(bool first) {
  srand(time(nullptr));
  int startIdx = 0;
  if (first) {
    for (FireParticle &p: particles) {
      initParticle(&p);
    }
  }
  else {
    startIdx = particles.size();
    for (int i = 0; i < density; i++) {
      FireParticle p = FireParticle();
      initParticle(&p);
      particles.push_back(p);
    }
  }
  // trail
  if (trail) {
    int particleNum = particles.size();
    subParticles.resize((int) particleNum * subNum);

    for (size_t i = startIdx; i < particleNum; ++i) {
      FireParticle &p = particles[i];
      for (size_t j = 0; j < subNum; ++j) {
        FireParticle &subP = subParticles[j * particleNum + i];
        subP.position = p.position;
        subP.velocity = p.velocity;
        subP.mass = p.mass;
        subP.lifetime = p.lifetime;
      }
    }
  }
}
