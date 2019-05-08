//
// Created by EBao on 2019-04-26.
//

#ifndef CLOTHSIM_FIREWORK_H
#define CLOTHSIM_FIREWORK_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "clothMesh.h"
#include "FireParticle.h"
#include <nanogui/nanogui.h>

using namespace CGL;
using namespace std;
using namespace nanogui;

enum FireworkStatus { IGNITING = 0, EXPLODING = 1, DIED = 2 };
enum FireworkShape { DISPERSED = 0, SPHERICAL = 1, SPARKLER = 2};

class Firework {
public:
    Firework(Vector3D startPos = Vector3D(0,0,0), Vector3D velocity = Vector3D(0,0,0), float density = 10.0,
             float energy = 3.0, float damping = 1.0, float particle_size = 10.0, bool blink = false,
             bool trail = false, FireworkShape shape = DISPERSED);
    ~Firework();
    void simulate(double frames_per_sec, double simulation_steps, vector<Vector3D> external_accelerations);
    void initExplosion(bool first = true);
    void initParticle(FireParticle *p);

    FireworkStatus status;
    vector<FireParticle> particles;
    vector<FireParticle> subParticles;
    FireParticle* igniteParticle;
    nanogui::Color color;
    float particle_size;

    // types
    bool blink;
    bool trail;
    FireworkShape shape;
    int subNum = 8;

    // for smoke
    Vector3D ignitePos;
    Vector3D igniteVel;
    int exploded;

private:
    float density;
    float energy;
    float damping;
    Vector3D  startVelocity;
    int collisionStep = 0;
//    float trailLen = 0.007;
  float trailLen = 0.03;
    int totalSteps = 0;
};


#endif //CLOTHSIM_FIREWORK_H