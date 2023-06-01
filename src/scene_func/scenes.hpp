#pragma once
#include "./renderManager.hpp"
SceneInfo cornell_box() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList objects;

  auto red = make_shared<Lambertian>(vec3(0.65, 0.05, 0.05));
  auto white = make_shared<Lambertian>(vec3(0.73f));
  auto green = make_shared<Lambertian>(vec3(0.12, 0.45, 0.15));
  auto light = make_shared<DiffuseEmitter>(vec3(1.0f), 15);

  objects.addObject(
      make_shared<AARectangle<Axis::Y>>(554, 213, 343, 227, 332, light));
  objects.addObject(
      make_shared<AARectangle<Axis::X>>(555, 0, 555, 0, 555, green));
  objects.addObject(make_shared<AARectangle<Axis::X>>(0, 0, 555, 0, 555, red));
  objects.addObject(
      make_shared<AARectangle<Axis::Y>>(0, 0, 555, 0, 555, white));
  objects.addObject(
      make_shared<AARectangle<Axis::Y>>(555, 0, 555, 0, 555, white));
  objects.addObject(
      make_shared<AARectangle<Axis::Z>>(555, 0, 555, 0, 555, white));

  shared_ptr<Hittable> box_left =
      make_shared<Box>(vec3(0.f), vec3(165, 330, 165), white);
  box_left = make_shared<Rotate<Axis::Y>>(box_left, gl::to_radian(15));
  box_left = make_shared<Translate>(box_left, vec3(265, 0, 295));
  objects.addObject(box_left);

  shared_ptr<Hittable> box_right =
      make_shared<Box>(vec3(0.f), vec3(165.f), white);
  box_right = make_shared<Rotate<Axis::Y>>(box_right, gl::to_radian(-18));
  box_right = make_shared<Translate>(box_right, vec3(130, 0, 65));
  objects.addObject(box_right);

  scene._height = 600;
  scene._width = 600;
  scene.spp_x = 10;
  scene.spp_y = 10;
  scene.GAMMA = 2.f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(40.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(0, 0, 1.f).normalize(),
      vec3(278.f, 278.f, -800.f));
  scene.objects = objects;
  scene.bg_color = vec3(0.f);

  return scene;
};

SceneInfo simple_light() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList objects;

  auto noise_text = make_shared<NoiseTexture>(10, 4);
  objects.addObject(make_shared<Sphere>(vec3(0, -1000, 0), 1000,
                                        make_shared<Lambertian>(noise_text)));
  objects.addObject(make_shared<Sphere>(vec3(0, 2, 0), 2,
                                        make_shared<Lambertian>(noise_text)));

  auto difflight = make_shared<DiffuseEmitter>(gl::DefaultTexture, 4);
  objects.addObject(
      make_shared<AARectangle<Axis::Z>>(-2, 3, 5, 1, 3, difflight));

  scene.objects = objects;
  scene._width = 1200;
  scene._height = 500;
  scene.spp_x = 4;
  scene.spp_y = 4;
  scene.GAMMA = 1.5f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(20.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(-26.f, -1.f, -6.f).normalize(),
      vec3(26.f, 3.f, 6.f));
  scene.bg_color = vec3(0.f);

  return scene;
}

SceneInfo random_scene() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList world;
  auto checker = make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1),
                                             vec3(0.9, 0.9, 0.9), 100);
  auto checker_mat = make_shared<Lambertian>(checker);

  auto noise = make_shared<NoiseTexture>(20, 4);
  auto ground_material = make_shared<Lambertian>(noise);

  world.addObject(make_shared<Sphere>(vec3(0, -1000, 0), 1000));

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

  scene.objects = world;
  scene._width = 1200;
  scene._height = 500;
  scene.spp_x = 3;
  scene.spp_y = 3;
  scene.GAMMA = 1.0f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(20.f), (float)(scene._width) / (float)(scene._height), 10.f,
      40.f, vec3(0, 1, 0), vec3(-13.f, -2.f, -3.f).normalize(),
      vec3(13.f, 2.f, 3.f));
  scene.bg_color = vec3(1.0f);

  return scene;
}
