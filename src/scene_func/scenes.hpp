#pragma once
#include "../base/material.hpp"
#include "../base/objectList.hpp"
#include "../base/primitive.hpp"
ObjectList random_scene() {
  using namespace std;
  using namespace gl;

  ObjectList world;
  auto checker = make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1),
                                             vec3(0.9, 0.9, 0.9), 100);
  auto checker_mat = make_shared<Lambertian>(checker);

  auto noise = make_shared<NoiseTexture>(20,4);
  auto ground_material = make_shared<Lambertian>(noise);

  world.addObject(
      make_shared<Sphere>(vec3(0, -1000, 0), 1000));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = C_rand();
      vec3 center(a + 0.9 * C_rand(), 0.2, b + 0.9 * C_rand());

      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<Material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = C_rand_vec3() * C_rand_vec3();
          sphere_material = make_shared<Lambertian>(albedo);
          world.addObject(make_shared<Sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // mirror
          auto albedo = C_rand_vec3(0.5, 1);
          auto fuzz = C_rand(0, 0.5);
          sphere_material = make_shared<Mirror>(albedo, fuzz);
          world.addObject(make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<Dielectric>(1.5);
          world.addObject(make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<Dielectric>(1.5);
  world.addObject(make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<Lambertian>(vec3(0.4, 0.2, 0.1));
  world.addObject(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<Mirror>(vec3(0.7, 0.6, 0.5), 0.0);
  world.addObject(make_shared<Sphere>(vec3(4, 1, 0), 1.0, ground_material));

  return world;
}