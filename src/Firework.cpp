//
// Created by EBao on 2019-04-26.
//

#include "Firework.h"

Firework::Firework(Vector3D startPos, Vector3D velocity, float density, float energy, float damping):density(density), energy(energy), damping(damping){
    status = FireworkStatus::IGNITING;
    igniteParticle = new FireParticle(startPos, velocity);
    std::cout<< "new fireworks here "<<std::endl;
}


Firework::~Firework() {}

void Firework::simulate(double frames_per_sec, double simulation_steps, vector<Vector3D> external_accelerations){
//    std::cout<< " firework simulate "<< std::endl;

    double delta_t = 1.0f / frames_per_sec / simulation_steps;

    if(status == DIED)
        return;
    if(status == IGNITING) {
        Vector3D lastPos = igniteParticle->position;
        Vector3D lastVec = igniteParticle->velocity;
        igniteParticle->position += igniteParticle->velocity * delta_t;
        for (auto ac: external_accelerations) {
            igniteParticle->position += ac * pow(delta_t, 2);
            igniteParticle->velocity += ac * delta_t;
        }
        if (igniteParticle->velocity.y < EPS_F) {
            status = EXPLODING;
            igniteParticle->position = lastPos;
            igniteParticle->velocity = lastVec;
            particles.resize((int) density);
            initExplosion();  //fireworks shape here
            delete(igniteParticle);
        }
    }

    if(status == EXPLODING) {
        for (FireParticle& p: particles) {
            p.lifetime -= delta_t;
            if(p.lifetime < EPS_F){
                status = DIED;
                continue;
            }
            p.position += p.velocity * delta_t;
            //add damping here  in or out ???
            Vector3D dampAc = -(p.velocity).unit() * damping * damping;
            p.velocity += dampAc * delta_t;
            p.position += dampAc * pow(delta_t, 2);
            //add ac here
            for (auto ac: external_accelerations) {
                p.velocity += ac * delta_t;
                p.position += ac * pow(delta_t, 2);
            }
        }
    }
}

double random_uniform() {
    if (rand()%2 == 0 )
        return ((double)std::rand()) / RAND_MAX;
    else
        return -((double)std::rand()) / RAND_MAX;
}

Vector3D random_uni_velocity(){
    Vector3D v(random_uniform(), random_uniform(), random_uniform());
    return v.unit();
}

void Firework::initExplosion(){
    srand (time(nullptr));
    for(FireParticle& p: particles){
        p.position = igniteParticle->position;
        p.velocity = igniteParticle->velocity;
        Vector3D v_dir = random_uni_velocity();
        p.mass = 1.0 + random_uniform();   // change
        p.velocity += v_dir * energy / p.mass;
        p.lifetime = 0.8 + 0.3 * random_uniform();  //change
    }
}