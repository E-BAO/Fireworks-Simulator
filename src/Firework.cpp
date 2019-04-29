//
// Created by EBao on 2019-04-26.
//

#include "Firework.h"

Firework::Firework(Vector3D startPos, Vector3D velocity, float density){
    status = FireworkStatus::IGNITING;
    ignitParticle = new FireParticle(startPos, velocity);
}


Firework::~Firework() {}

void Firework::simulate(double frames_per_sec, double simulation_steps, vector<Vector3D> external_accelerations){
    switch(status){
        case DIED:
            break;
        case IGNITING:
            double delta_t = 1.0f / frames_per_sec / simulation_steps;
            for(auto ac: external_accelerations){
                ignitParticle->position +=  ac * pow(delta_t, 2);
                ignitParticle->velocity +=  ac * delta_t;
            }
            if(ignitParticle->velocity.y < 0){
                status = EXPLODING;
                particles.resize((int)density);
                initExplosion();
            }
            break;
        case EXPLODING:
            break;
    }
}

void Firework::initExplosion(){

}

void Firework::draw(GLShader &shader) {

}
