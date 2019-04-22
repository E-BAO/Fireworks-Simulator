#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.

    for(int y = 0; y < num_height_points; y ++){
        for(int x = 0; x < num_width_points; x ++){
            vector<int> mass_set = {x,y};
            vector<vector<int>>::iterator it = find(pinned.begin(), pinned.end(), mass_set);
            bool pin =  it != pinned.end() ? true: false;
            Vector3D position;
            if(orientation == HORIZONTAL){
                position.x = (double)(x * width) / (double)(num_width_points - 1);
                position.y = 1;
                position.z = (double)(y * height) / (double)(num_height_points - 1);
            }else if(orientation == VERTICAL){
                position.x = (double)(x * width) / (double)(num_width_points - 1);
                position.y = (double)(y * height) / (double)(num_height_points - 1);
                position.z = (double)rand() / (double)RAND_MAX * 0.002 - 0.001;
            }
            point_masses.emplace_back(position, pin);
        }
    }
    
    for(int y = 0; y < num_height_points; y ++){
        for(int x = 0; x < num_width_points; x ++){
            PointMass *pm = &point_masses[y * num_width_points + x];
            if(x > 0){
                PointMass *pm_left = pm - 1;
                springs.emplace_back(pm, pm_left, STRUCTURAL);
                
                if(x > 1){
                    PointMass *pm_le_left = pm_left - 1;
                    springs.emplace_back(pm, pm_le_left, BENDING);
                }
            }
            
            if(y > 0){
                PointMass *pm_up = pm - num_width_points;
                springs.emplace_back(pm, pm_up, STRUCTURAL);
                
                if(x > 0){
                    PointMass *pm_up_left = pm_up - 1;
                    springs.emplace_back(pm, pm_up_left, SHEARING);
                }
                if(x < num_width_points - 1){
                    PointMass *pm_up_right = pm_up + 1;
                    springs.emplace_back(pm, pm_up_right, SHEARING);
                }
                if( y > 1){
                    PointMass *pm_up_up = pm_up - num_width_points;
                    springs.emplace_back(pm, pm_up_up, BENDING);
                }
            }
        }
    }

}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.

    for(int y = 0; y < num_height_points; y ++){
        for(int x = 0; x < num_width_points; x ++){
            PointMass *pm = &point_masses[y * num_width_points + x];
            pm->forces = Vector3D(0,0,0);
            for(Vector3D ac:external_accelerations){
                pm->forces += ac * mass;
            }
        }
    }
    
    for(vector<Spring>::iterator s_it = springs.begin(); s_it != springs.end(); s_it ++){
        if((s_it->spring_type == STRUCTURAL && cp->enable_structural_constraints) ||
           (s_it->spring_type == SHEARING && cp->enable_shearing_constraints)){
            double fs = cp->ks * ((s_it->pm_a->position - s_it->pm_b->position).norm() - s_it->rest_length);
            Vector3D fs_dir = (s_it->pm_b->position - s_it->pm_a->position).unit() * fs;
            s_it->pm_a->forces += fs_dir;
            s_it->pm_b->forces -= fs_dir;
        }
        
        if(s_it->spring_type == BENDING && cp->enable_bending_constraints){
            double fs = cp->ks * ((s_it->pm_a->position - s_it->pm_b->position).norm() - s_it->rest_length) * 0.2;
            Vector3D fs_dir = (s_it->pm_b->position - s_it->pm_a->position).unit() * fs;
            s_it->pm_a->forces += fs_dir;
            s_it->pm_b->forces -= fs_dir;
        }
    }
  // TODO (Part 2): Use Verlet integration to compute new point mass positions


    for(int y = 0; y < num_height_points; y ++){
        for(int x = 0; x < num_width_points; x ++){
            PointMass *pm = &point_masses[y * num_width_points + x];
            if(pm->pinned)
                continue;
            Vector3D tempPos = pm->position;
            pm->position = pm->position + (1.0 - cp->damping * 0.01) * (pm->position - pm->last_position) + pm->forces / mass * pow(delta_t, 2.0);
            pm->last_position = tempPos;
        }
    }
  // TODO (Part 4): Handle self-collisions.
    build_spatial_map();
    for(int y = 0; y < num_height_points; y ++){
        for(int x = 0; x < num_width_points; x ++){
            PointMass *pm = &point_masses[y * num_width_points + x];
            self_collide(*pm, simulation_steps);
        }
    }


  // TODO (Part 3): Handle collisions with other primitives.
    for(int y = 0; y < num_height_points; y ++){
        for(int x = 0; x < num_width_points; x ++){
            PointMass *pm = &point_masses[y * num_width_points + x];
            for(CollisionObject* ob : (*collision_objects)){
                ob->collide(*pm);
            }
        }
    }

  // TODO (Part 2): Constrain the changes to be such that the spring does not change
    for(vector<Spring>::iterator s_it = springs.begin(); s_it != springs.end(); s_it ++){
        Vector3D dir_btoa = s_it->pm_a->position - s_it->pm_b->position;
        double delta_l = dir_btoa.norm() - s_it->rest_length;
        if(delta_l > s_it->rest_length * 0.1){
            double delta_len = delta_l - s_it->rest_length * 0.1;
            Vector3D delta_pos = delta_len * dir_btoa.unit() * 0.5;
            if(!s_it->pm_a->pinned && !s_it->pm_b->pinned){
                s_it->pm_a->position -= delta_pos;
                s_it->pm_b->position += delta_pos;
            }else if(!s_it->pm_a->pinned){
                s_it->pm_a->position -= delta_pos * 2.0;
            }else if(!s_it->pm_b->pinned){
                s_it->pm_b->position += delta_pos * 2.0;
            }
        }
    }
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();
  
  // TODO (Part 4): Build a spatial map out of all of the point masses.
    for(int y = 0; y < num_height_points; y ++){
        for(int x = 0; x < num_width_points; x ++){
            PointMass *pm = &point_masses[y * num_width_points + x];
            float hash = hash_position(pm->position);
        if(!map.count(hash)){
          map[hash] = new vector<PointMass *>();
        }
        map[hash]->push_back(pm);
    }
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
    Vector3D correct(0,0,0);

    float hash = hash_position(pm.position);

    if(map.count(hash)){
        int count = 0;
        for(PointMass *p: *map[hash]){
            if(p == &pm)
                continue;
            Vector3D dir = pm.position - p->position;
            double dist = dir.norm();
            if(dist < thickness * 2.0){
                double delta_l =  thickness * 2.0 - dist;
                dir.normalize();
                correct += delta_l * dir;
                count ++;
            }
        }
        if(count > 0){
          correct /= count;
          correct /= simulation_steps;
          pm.position += correct;
        }
    }

}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
    double w = 3 * width / num_width_points;
    double h = 3 * height / num_height_points;
    double t = max(w, h);
    
    int a = pos.x / w;
    int b = pos.y / h;
    int c = pos.z / t;

    return a + b * num_width_points + c * num_width_points * num_height_points;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
