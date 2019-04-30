#ifndef COLLISIONOBJECT_PLANE_H
#define COLLISIONOBJECT_PLANE_H

#include <nanogui/nanogui.h>

#include "../ray.h"
#include "../clothMesh.h"
#include "collisionObject.h"
#include "intersection.h"

using namespace nanogui;
using namespace CGL;
using namespace std;

struct Plane : public CollisionObject {
public:
  Plane(const Vector3D &point, const Vector3D &normal, double friction)
      : point(point), normal(normal.unit()), friction(friction) {
    Vector3D v_parallel(normal.y - normal.z, normal.z - normal.x, normal.x - normal.y);
    v_parallel.normalize();
    Vector3D v_cross = cross(normal, v_parallel);
    p0 = point + 2 * ( v_cross + v_parallel);
    p1 = point + 2 * ( v_cross - v_parallel);
    p2 = point + 2 * (-v_cross + v_parallel);
    p3 = point + 2 * (-v_cross - v_parallel);
  }

  void render(GLShader &shader);
  void collide(PointMass &pm);
  Vector3D mollerTrumbore(const Ray &r, const Vector3D &v0,
      const Vector3D &v1, const Vector3D &v2);
  bool intersectTriangle(const Ray &r, Intersection *isect,
      const Vector3D &v0, const Vector3D &v1, const Vector3D &v2);
  bool intersect(const Ray &r, Intersection *isect);

  Vector3D point;
  Vector3D normal;
  Vector3D p0, p1, p2, p3;

  double friction;

};

#endif /* COLLISIONOBJECT_PLANE_H */
