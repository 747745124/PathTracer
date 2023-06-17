#pragma once
#include "../utils/colors.hpp"
#include "./renderManager.hpp"

SceneInfo cornell_ckpt10(){

};

SceneInfo cornell_box() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList objects;
  //used for shadowray
  LightList lights;
  

  auto red = make_shared<Lambertian>(vec3(0.65, 0.05, 0.05));
  auto white = make_shared<Lambertian>(vec3(0.73f));
  auto green = make_shared<Lambertian>(vec3(0.12, 0.45, 0.15));
  auto light = make_shared<DiffuseEmitter>(vec3(1.0f), 15);
  std::array<gl::vec3, 4> vertices;
  vertices[0] = {213, 554, 227};
  vertices[1] = {343, 554, 227};
  vertices[2] = {343, 554, 332};
  vertices[3] = {213, 554, 332};
  lights.addLight(make_shared<QuadLight>(vertices, vec3(1.0f), 15));

  objects.addObject(make_shared<FlipFace>(
      make_shared<AARectangle<Axis::Y>>(554, 213, 343, 227, 332, light)));
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
  box_left = make_shared<Rotate<Axis::Y>>(box_left, gl::to_radian(15.f));
  box_left = make_shared<Translate>(box_left, vec3(265, 0, 295));
  objects.addObject(box_left);

  shared_ptr<Hittable> box_right =
      make_shared<Box>(vec3(0.f), vec3(165.f), white);
  box_right = make_shared<Rotate<Axis::Y>>(box_right, gl::to_radian(-18.f));
  box_right = make_shared<Translate>(box_right, vec3(130, 0, 65));

  objects.addObject(box_right);

  scene._height = 300;
  scene._width = 300;
  scene.spp_x = 10;
  scene.spp_y = 10;
  scene.GAMMA = 2.f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(40.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(0, 0, 1.f).normalize(),
      vec3(278.f, 278.f, -800.f));
  scene.objects = objects;
  scene.bg_color = vec3(0.f);
  scene.lights = lights;
  return scene;
};

SceneInfo two_lights() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList objects;
  LightList lights;

  auto noise_text = make_shared<NoiseTexture>(10, 4);
  objects.addObject(make_shared<Sphere>(vec3(0, -1000, 0), 1000,
                                        make_shared<Lambertian>(noise_text)));

  auto light_a_mat = make_shared<DiffuseEmitter>(gl::RED, 3);
  auto light_b_mat = make_shared<DiffuseEmitter>(gl::YELLOW, 10.f);

  auto light_a = make_shared<AARectangle<Axis::Y>>(1, -1, 2, 0, 3, light_a_mat);
  auto light_b = make_shared<Sphere>(vec3(2, 1, 0), 0.5f, light_b_mat);
  objects.addObject(light_a);
  objects.addObject(light_b);

  lights.addLight(make_shared<QuadLight>(light_a, gl::RED, 3));
  lights.addLight(make_shared<SphereLight>(light_b, gl::YELLOW, 10.f));


  scene.objects = objects;
  scene._width = 700;
  scene._height = 500;
  scene.spp_x = 2;
  scene.spp_y = 2;
  scene.GAMMA = 1.5f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(20.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(-26.f, -1.f, -6.f).normalize(),
      vec3(22.f, 3.f, 6.f));
  scene.bg_color = vec3(0.f);
  scene.lights = lights;

  return scene;
};

SceneInfo checkpoint_3() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList objects;
  LightList lights;

  auto ground = make_shared<Lambertian>(vec3(0.48f, 0.83f, 0.53f));
  auto mini_light = make_shared<DiffuseEmitter>(gl::RED_PLASTIC, 1);

  ObjectList boxes1;
  const int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      auto w = 100.0;
      auto x0 = -1000.0 + i * w;
      auto z0 = -1000.0 + j * w;
      auto y0 = 0.0;
      auto x1 = x0 + w;
      auto y1 = C_rand(1.f, 101.f);
      auto z1 = z0 + w;

      if ((i + j) % 2 == 0) {
        boxes1.addObject(
            make_shared<Box>(vec3(x0, y0, z0), vec3(x1, y1, z1), mini_light));
        auto light_obj =
            make_shared<XZRectangle>(y1, x0, x1, z0, z1, mini_light);
        lights.addLight(make_shared<QuadLight>(light_obj, gl::RED_PLASTIC, 1));
      } else {
        boxes1.addObject(
            make_shared<Box>(vec3(x0, y0, z0), vec3(x1, y1, z1), ground));
      }
    }
  }

  objects.addObject(make_shared<BVHNode>(boxes1, 0, 1));

  auto light_mat = make_shared<DiffuseEmitter>(gl::WHITE, 1.f);
  auto light_obj = make_shared<XZRectangle>(700, 123, 423, 147, 412, light_mat);
  objects.addObject(light_obj);
  lights.addLight(make_shared<QuadLight>(light_obj, gl::WHITE, 1.f));

  auto sphere_light_mat = make_shared<DiffuseEmitter>(gl::BRONZE, 3.f);
  auto sphere_light =
      make_shared<Sphere>(vec3(250, 150, 160), 50, sphere_light_mat);
  objects.addObject(sphere_light);
  lights.addLight(make_shared<SphereLight>(sphere_light, gl::BRONZE, 3.f));

  scene.objects = objects;
  scene._width = 400;
  scene._height = 400;
  scene.spp_x = 10;
  scene.spp_y = 10;
  scene.GAMMA = 2.0f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(40.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(-200.f, 0.f, 600.f).normalize(),
      vec3(478.f, 278.f, -600.f));
  scene.bg_color = vec3(0.f);
  scene.lights = lights;
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

  auto difflight = make_shared<DiffuseEmitter>(gl::DefaultTexture, 4);
  objects.addObject(
      make_shared<AARectangle<Axis::Y>>(4, 2, 5, 0, 3, difflight));

  scene.objects = objects;
  scene._width = 700;
  scene._height = 500;
  scene.spp_x = 2;
  scene.spp_y = 2;
  scene.GAMMA = 1.5f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(20.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(-26.f, -1.f, -6.f).normalize(),
      vec3(22.f, 3.f, 6.f));
  scene.bg_color = vec3(0.f);

  return scene;
};

SceneInfo night_time() {
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

SceneInfo checkpoint_2() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList objects;
  LightList lights;

  auto ground = make_shared<Lambertian>(vec3(0.48f, 0.83f, 0.53f));

  ObjectList boxes1;
  const int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      auto w = 100.0;
      auto x0 = -1000.0 + i * w;
      auto z0 = -1000.0 + j * w;
      auto y0 = 0.0;
      auto x1 = x0 + w;
      auto y1 = C_rand(1.f, 101.f);
      auto z1 = z0 + w;

      boxes1.addObject(
          make_shared<Box>(vec3(x0, y0, z0), vec3(x1, y1, z1), ground));
    }
  }

  objects.addObject(make_shared<BVHNode>(boxes1, 0, 1));

  auto light = make_shared<DiffuseEmitter>(vec3(1.f), 7);
  auto light_obj = make_shared<XZRectangle>(554, 123, 423, 147, 412, light);
  objects.addObject(light_obj);
  lights.addLight(make_shared<QuadLight>(light_obj, vec3(1.f), 7));

  auto center1 = vec3(400, 400, 200);
  auto center2 = center1 + vec3(30, 0, 0);
  auto moving_sphere_material = make_shared<Lambertian>(vec3(0.7, 0.3, 0.1));
  objects.addObject(make_shared<Sphere>(center1, 50, moving_sphere_material));

  objects.addObject(make_shared<Sphere>(vec3(260, 150, 45), 50,
                                        make_shared<Dielectric>(1.5)));
  objects.addObject(make_shared<Sphere>(
      vec3(0, 150, 145), 50, make_shared<Mirror>(vec3(0.8, 0.8, 0.9), 1.0)));

  auto boundary = make_shared<Sphere>(vec3(360, 150, 145), 70,
                                      make_shared<Dielectric>(1.5));
  objects.addObject(boundary);
  objects.addObject(
      make_shared<ConstantMedium>(boundary, 0.2, vec3(0.2, 0.4, 0.9)));
  boundary =
      make_shared<Sphere>(vec3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
  objects.addObject(make_shared<ConstantMedium>(boundary, .0001f, vec3(1.f)));

  auto emat = make_shared<Lambertian>(
      make_shared<ImageTexture>("../assets/textures/ao.png"));
  objects.addObject(make_shared<Sphere>(vec3(400, 200, 400), 100, emat));
  auto pertext = make_shared<NoiseTexture>(100);
  objects.addObject(make_shared<Sphere>(vec3(220, 280, 300), 80,
                                        make_shared<Lambertian>(pertext)));

  ObjectList boxes2;
  auto white = make_shared<Lambertian>(vec3(.73));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.addObject(
        make_shared<Sphere>(gl::C_rand_vec3(0.f, 165.f), 10, white));
  }

  objects.addObject(make_shared<Translate>(
      make_shared<Rotate<Axis::Y>>(make_shared<BVHNode>(boxes2, 0.0, 1.0),
                                   gl::to_radian(15)),
      vec3(-100, 270, 395)));

  scene.objects = objects;
  scene._width = 400;
  scene._height = 400;
  scene.spp_x = 10;
  scene.spp_y = 10;
  scene.GAMMA = 2.0f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(40.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(-200.f, 0.f, 600.f).normalize(),
      vec3(478.f, 278.f, -600.f));
  scene.bg_color = vec3(0.f);
  scene.lights = lights;
  return scene;
};

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

SceneInfo night() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList world;
  LightList lights;

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
        // diffuse
        auto albedo = C_rand_vec3() * C_rand_vec3();
        sphere_material = make_shared<Lambertian>(albedo);
        world.addObject(make_shared<Sphere>(center, 0.2, sphere_material));
      }
    }
  }

  auto material = make_shared<DiffuseEmitter>(vec3(0.4, 0.2, 0.1), 6.f);
  world.addObject(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material));
  lights.addLight(
      make_shared<SphereLight>(vec3(-4, 1, 0), 1.0, vec3(0.4, 0.2, 0.1), 6.f));

  scene.objects = world;
  scene._width = 720;
  scene._height = 480;
  scene.spp_x = 5;
  scene.spp_y = 5;
  scene.GAMMA = 1.0f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(20.f), (float)(scene._width) / (float)(scene._height), 20.f,
      100.f, vec3(0, 1, 0), vec3(-13.f, -2.f, -3.f).normalize(),
      vec3(13.f, 2.f, 3.f));
  scene.lights = lights;
  scene.bg_color = vec3(0.0f);
  return scene;
};

SceneInfo NightTime() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList world;
  LightList lights;

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
        // diffuse
        auto albedo = C_rand_vec3() * C_rand_vec3();
        sphere_material = make_shared<Lambertian>(albedo);
        world.addObject(make_shared<Sphere>(center, 0.2, sphere_material));
      }
    }
  }

  auto material = make_shared<DiffuseEmitter>(vec3(0.4, 0.2, 0.1), 6.f);
  world.addObject(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material));
  lights.addLight(
      make_shared<SphereLight>(vec3(-4, 1, 0), 1.0, vec3(0.4, 0.2, 0.1), 6.f));

  scene.objects = world;
  scene._width = 720;
  scene._height = 480;
  scene.spp_x = 5;
  scene.spp_y = 5;
  scene.GAMMA = 1.0f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(20.f), (float)(scene._width) / (float)(scene._height), 20.f,
      100.f, vec3(0, 1, 0), vec3(-13.f, -2.f, -3.f).normalize(),
      vec3(13.f, 2.f, 3.f));
  scene.lights = lights;
  scene.bg_color = vec3(0.0f);
  return scene;
};

SceneInfo DiffuseDiffuse() {
  using namespace std;
  using namespace gl;

  SceneInfo scene;
  ObjectList world;
  LightList lights;

  auto material = make_shared<DiffuseEmitter>(vec3(1.f), 5.f);
  world.addObject(make_shared<Sphere>(vec3(0, 1.8, 0), 1.0, material));
  lights.addLight(
      make_shared<SphereLight>(vec3(0, 1.8, 0), 1.0, vec3(1.f), 5.f));

  auto material_ground = make_shared<Lambertian>(gl::GREEN);
  auto material_center = make_shared<Lambertian>(vec3(0.7, 0.3, 0.3));
  auto material_left = make_shared<Lambertian>(vec3(0.8, 0.8, 0.8));
  auto material_right = make_shared<Lambertian>(vec3(0.8, 0.6, 0.2));


  world.addObject(
      make_shared<Sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
  world.addObject(make_shared<Sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
  world.addObject(make_shared<Sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
  world.addObject(make_shared<Sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));
  world.addObject(make_shared<XYRectangle>(-1.8, -1.8, 1.8, -1, 1, material_right));
  world.addObject(make_shared<YZRectangle>(-1.8, -1, 1, -1.8, 0.5, material_left));
  world.addObject(make_shared<YZRectangle>(1.8, -1, 1, -1.8, 0.5, material_center));
  world.addObject(make_shared<XZRectangle>(1, -1.8, 1.8, -1.8, 0.5, material_center));

  scene.objects = world;
  scene._width = 540;
  scene._height = 360;
  scene.spp_x = 60;
  scene.spp_y = 60;
  scene.GAMMA = 1.0f;
  scene.camera = make_shared<PerspectiveCamera>(
      gl::to_radian(50.f), (float)(scene._width) / (float)(scene._height), 10.f,
      1000.f, vec3(0, 1, 0), vec3(0,0,-1).normalize(),
      vec3(0,0,2));
  scene.lights = lights;
  scene.bg_color = vec3(0.0f);
  return scene;
};
