//
// Created by EBao on 2019-04-26.
//

#include "Firework.h"

Firework::Firework(Vector3D startPos, Vector3D velocity, float density, float energy, float damping):density(density), energy(energy), damping(damping){
    status = FireworkStatus::IGNITING;
    ignitParticle = new FireParticle(startPos, velocity);
}


Firework::~Firework() {}

void Firework::simulate(double frames_per_sec, double simulation_steps, vector<Vector3D> external_accelerations){
    double delta_t = 1.0f / frames_per_sec / simulation_steps;

    if(status == DIED)
        return;
    if(status == IGNITING) {
        Vector3D lastPos = ignitParticle->position;
        Vector3D lastVec = ignitParticle->velocity;
        for (auto ac: external_accelerations) {
            ignitParticle->velocity += ac * delta_t;
            ignitParticle->position += ac * pow(delta_t, 2);
        }
        if (ignitParticle->velocity.y < EPS_F) {
            status = EXPLODING;
            ignitParticle->position = lastPos;
            ignitParticle->velocity = lastVec;
            particles.resize((int) density);
            initExplosion();  //fireworks shape here
            delete(ignitParticle);
        }
    }

    if(status == EXPLODING) {
        for (auto p: particles) {
            p->lifetime -= delta_t;
            if(p->lifetime < EPS_F)
                continue;
            //add damping here  in or out ???
            Vector3D dampAc = -(p->velocity).unit() * damping;
            p->velocity += dampAc * delta_t;
            p->position += dampAc * pow(delta_t, 2);
            //add ac here
            for (auto ac: external_accelerations) {
                p->velocity += ac * delta_t;
                p->position += ac * pow(delta_t, 2);
            }
        }
    }
}

double random_uniform() {
    return ((double)std::rand()) / RAND_MAX;
}

Vector3D random_uni_velocity(){
    Vector3D v(random_uniform(), random_uniform(), random_uniform());
    return v.unit();
}

void Firework::initExplosion(){
    srand (time(nullptr));
    for(auto p: particles){
        p = new FireParticle(ignitParticle->position, ignitParticle->velocity);
        Vector3D v_dir = random_uni_velocity();
        p->mass = 1.0 + random_uniform();   // change
        p->velocity += v_dir * energy / p->mass;
        p->lifetime = 0.8 + 0.3 * random_uniform();  //change
    }
}

void Firework::draw(GLShader &shader) {
    for(auto p: particles){

    }

}
