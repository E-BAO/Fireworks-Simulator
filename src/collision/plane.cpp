#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../clothSimulator.h"
#include "plane.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void Plane::collide(PointMass &pm) {
  Vector3D p2p = pm.position - point;
  Vector3D lp2p = pm.last_position - point;
  if (dot(p2p, normal) * dot(lp2p, normal) < 0) {
    Vector3D intersect = pm.position + dot(-p2p, normal) * normal;
    Vector3D correct = (intersect - pm.last_position);
    correct = dot(correct, normal) < 0 ? correct + SURFACE_OFFSET * normal : correct - SURFACE_OFFSET * normal;
    pm.position = pm.last_position + correct * (1.0 - friction);
  }
}

void Plane::render(GLShader &shader) {
  nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);

  Vector3f sPoint(point.x, point.y, point.z);
  Vector3f sNormal(normal.x, normal.y, normal.z);
  Vector3f sParallel(normal.y - normal.z, normal.z - normal.x,
                     normal.x - normal.y);
  sParallel.normalize();
  Vector3f sCross = sNormal.cross(sParallel);

  MatrixXf positions(3, 4);
  MatrixXf normals(3, 4);
  MatrixXf blink_states = MatrixXf::Ones(1, 4);

  positions.col(0) << sPoint + 2 * (sCross + sParallel);
  positions.col(1) << sPoint + 2 * (sCross - sParallel);
  positions.col(2) << sPoint + 2 * (-sCross + sParallel);
  positions.col(3) << sPoint + 2 * (-sCross - sParallel);

  normals.col(0) << sNormal;
  normals.col(1) << sNormal;
  normals.col(2) << sNormal;
  normals.col(3) << sNormal;

  if (shader.uniform("u_color", false) != -1) {
      shader.setUniform("u_color", color);
  }
  shader.uploadAttrib("in_position", positions);
  if (shader.attrib("in_normal", false) != -1) {
      shader.uploadAttrib("in_normal", normals);
  }
  if (shader.attrib("in_blink", false) != -1) {
      shader.uploadAttrib("in_blink", blink_states);
  }

  shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
}

Vector3D Plane::mollerTrumbore(const Ray &r, const Vector3D &v0, const Vector3D &v1, const Vector3D &v2) {

  Vector3D e1, e2, s, s1, s2;
  e1 = v1 - v0;
  e2 = v2 - v0;
  s = r.o - v0;
  s1 = cross(r.d, e2);
  s2 = cross(s, e1);

  Vector3D result = Vector3D(dot(s2, e2), dot(s1, s), dot(s2, r.d)) / dot(s1, e1);
  return result;
}

bool Plane::intersectTriangle(const Ray &r, Intersection *isect,
                              const Vector3D &v0, const Vector3D &v1, const Vector3D &v2) {

  Vector3D bary_result = mollerTrumbore(r, v0, v1, v2);
  double t, b1, b2;
  t = bary_result[0];
  b1 = bary_result[1];
  b2 = bary_result[2];

  if (b1 >= 0 and b1 <= 1 and b2 >= 0 and b2 <= 1 and b1 + b2 >= 0 and b1 + b2 <= 1
      and t >= r.min_t and t <= r.max_t) {
    isect->t = t;
    r.max_t = t;
    return true;
  }

  return false;
}

bool Plane::intersect(const Ray &r, Intersection *isect) {
  if (intersectTriangle(r, isect, p0, p1, p2))
    return true;
  if (intersectTriangle(r, isect, p1, p2, p3))
    return true;
  return false;
}
