#include <iostream>
#include <fstream>      // std::ofstream

#include <string.h>
#include <stdio.h>

#include "camera.h"
#include "material.h"
#include "sphere.h"
#include "hittable.h"
#include "float.h" /* inc. by ray as well, but written here for clarity */

hitable *list;
int len;

vec3 color(const ray& r, hitable *world, int depth) {
  hit_record rec;
  if (world->hit(r, 0.0001, FLT_MAX, rec)) {
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation*color(scattered, world, depth+1);
    } else { // background
      return vec3(0, 0, 0);
    }
  } else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5, 0.7, 1.0);
  }
}

// void make_list(){
//   len = 5;
//   list = malloc(5*sizeof(hitable *));
//
//   list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
//
//   list[2] = new sphere(vec3(1,0,-1), 0.5, new diaelectric(1.5));
//   list[4] = new sphere(vec3(0.5,1.5,-1), 0.4, new metal(vec3(0.1, 0.1, 0.8), 0.1));
//
//   list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8, 0.3, 0.3), 0.1));
//
//   list[1] = new sphere(vec3(0,-100.5,-1), 100, new metal(vec3(0.8, 0.7, 0.3), 0.5));
// }

int fill(hitable *l[]){
  // format

  // NUM OBJS
  // OBJECT TYPE
  // OBJECT SPEC
  // rep
  char *curr_line;
  int bytes_read;
  size_t nbytes = 1000;
  curr_line = (char *)malloc(nbytes + 1);
  bytes_read = getline(&curr_line, &nbytes, stdin);

  int num_objs;
  sscanf(curr_line, "%i", &num_objs);

  for (int i = 0; i < num_objs; i++){
    std::cout << "line no: " << i << "\n";
    getline(&curr_line, &nbytes, stdin);
    if (strncmp("sphere", curr_line, 6) == 0){
      getline(&curr_line, &nbytes, stdin);
      if (strncmp("glass", curr_line, 5)  == 0) {
        float x, y, z;
        float lx, ly, lz;
        float size;
        getline(&curr_line, &nbytes, stdin);
        sscanf(curr_line, "%f %f %f %f %f %f %f", &x, &y, &z, &size, &lx, &ly, &lz);
        l[i] = new sphere(vec3(x,y,z), size, new lambertian(vec3(lx,ly,lz)));
      } else if (strncmp("metal", curr_line, 5)  == 0) {
        float x, y, z;
        float lx, ly, lz;
        float size;
        getline(&curr_line, &nbytes, stdin);
        sscanf(curr_line, "%f %f %f %f %f %f %f", &x, &y, &z, &size, &lx, &ly, &lz);
        l[i] = new sphere(vec3(x,y,z), size, new metal(vec3(lx,ly,lz), 0.05));
      }
    }

  }
  return num_objs;
}

int main(int argc, char* argv[]) {
  std::ofstream outfile(argv[1], std::ofstream::out);
  int nx = 450;
  int ny = 300;
  int ns = 50;
  outfile << "P3\n" << nx << " " << ny << "\n255\n";

  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horizontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  vec3 origin(0.0, 0.0, 0.0);

  hitable *list[1000];
  int num_objs = fill(list);

  hitable *world = new hitable_list(list,num_objs);
  camera cam(vec3(0,0,-20), vec3(0,0,-1), vec3(0,1,0), 90, float(nx)/float(ny));

  for (int j = ny-1; j >= 0; j--){
    for (int i = 0; i < nx; i++) {
      vec3 col(0,0,0);
      for (int s = 0; s < ns; s++){
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);

        ray r = cam.get_ray(u, v);
        vec3 p = r.point_at(2.0);
        col = col + color(r, world, 0);
      }
      col = col / float(ns);
      col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); // gamma one-half
      int ir = int(255.99*col[0]);
      int ig = int(255.99*col[1]);
      int ib = int(255.99*col[2]);
      outfile << ir << " " << ig << " " << ib << "\n";
    }
  }
  outfile.close();
}
