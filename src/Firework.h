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

class Firework {
public:
    Firework(Vector3D startPos = Vector3D(0,0,0), Vector3D velocity = Vector3D(0,0,0), float density = 10.0, float energy = 3.0, float damping = 1.0);
    ~Firework();
    void simulate(double frames_per_sec, double simulation_steps, vector<Vector3D> external_accelerations);
    void initExplosion();

    FireworkStatus status;
    vector<FireParticle> particles;
    FireParticle* igniteParticle;
    nanogui::Color color;

private:
    float density;
    float energy;
    float damping;
};


#endif //CLOTHSIM_FIREWORK_H
