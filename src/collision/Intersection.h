#ifndef CGL_INTERSECT_H
#define CGL_INTERSECT_H

#include <vector>

#include "CGL/vector3D.h"

struct Intersection {

  Intersection() : t (INF_D) { }

  double t;    ///< time of intersection
};

#endif // CGL_INTERSECT_H
