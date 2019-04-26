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

class Firework {
public:
    Firework();
    ~Firework();
    void simulate(double frames_per_sec, double simulation_steps, vector<Vector3D> external_accelerations);
    void draw(GLShader &shader);

private:
    vector<FireParticle> particles;
};


#endif //CLOTHSIM_FIREWORK_H
