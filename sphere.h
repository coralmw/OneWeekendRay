#ifndef SPHEREH
#define SPHEREH

#include "hit.h"
#include "material.h"

class sphere: public hitable {
public:
  sphere() {}
  sphere(vec3 cen, float r, material *m) : center(cen), radius(r) { mat_ptr = m; };
  virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
  vec3 center;
  float radius;
  material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius*radius;
  float discrim =  b*b - a*c;
  if (discrim > 0) {
    /* intercept location, quad. formula for near cam */
    float temp = (-b - sqrt(b*b-a*c))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    /* intercept location, quad. formula for far cam */
    temp = (-b + sqrt(b*b-a*c))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

#endif
