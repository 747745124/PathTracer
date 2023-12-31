#pragma once
#include "../base/camera.hpp"
#include "../base/compound.hpp"
#include "../base/framebuffer.hpp"
#include "../base/material.hpp"
#include "../base/medium.hpp"
#include "../base/objectList.hpp"
#include "../base/primitive.hpp"
#include "../method/analytical_illumin.hpp"
#include "../method/maxdepth_shadowray.hpp"
#include "../method/maxdepth_naive.hpp"
#include "../method/roulette_naive.hpp"
#include "../utils/bvh.hpp"
#include "../utils/objectTransform.hpp"
#include "../utils/timeit.hpp"

extern uint64_t hit_count;

static std::vector<gl::vec3> light_vertices = {
    {2, 4, 3}, {4, 4, 3}, {4, 4, 0}, {2, 4, 0}};
static gl::vec3 light_color = {1.0f, 1.0f, 1.0f};
static float light_intensity = 6.0f;
static PolyLightInfo light_info = {light_vertices, light_color,
                                   light_intensity};

static auto default_light = std::make_shared<QuadLight>(light_info);
static LightList default_lights = {default_light};


struct SceneInfo {
  std::shared_ptr<PerspectiveCamera> camera = nullptr;
  std::shared_ptr<BVHNode> bvh = nullptr;
  ObjectList objects;
  ObjectList light_objects;
  gl::vec3 bg_color = gl::vec3(0.7, 0.8, 1.0);
  bool use_bvh = true;
  uint _width = 800;
  uint _height = 800;
  uint spp_x = 2;
  uint spp_y = 2;
  float GAMMA = 1.0f;
  LightList lights = {std::make_shared<QuadLight>(light_info)};


  SceneInfo() = default;

  void render(const std::string &out_path = "./output.png") {

    using namespace gl;
    using namespace std;

    if (camera == nullptr) {
      std::cout << "Camera is not initialized!" << std::endl;
      return;
    }

    if (objects.getLists().size() == 0) {
      std::cout << "No objects in the scene!" << std::endl;
      return;
    }

    if (use_bvh)
      bvh = make_shared<BVHNode>(objects);

    FrameBuffer fb(_width, _height, spp_x, spp_y);
    auto offsets = fb.getOffsets();
    uint counter = 0;

    light_objects = ObjectList(lights);

#pragma omp parallel for
    {
      for (int i = 0; i < _width; i++) {
        std::cout << "Now scanning " << (float(counter) / _width) * 100.f
                  << " %" << std::endl;

        for (int j = 0; j < _height; j++) {
          auto color = vec3(0.0);
          for (int k = 0; k < fb.getSampleCount(); k++) {
            auto sample_color = vec3(0.0);
            vec2 uv = (vec2(i, j) + offsets[k]) / vec2(_width, _height);
            Ray ray = camera->generateRay(uv.u(), uv.v());

#ifdef USE_ANALYTICAL_ILLUMIN
            color += getRayColor(ray, objects, bg_color, lights, bvh);
#elif defined USE_MAXDEPTH_SHADOWRAY
            color += getRayColor(ray, objects, bg_color, lights, 40, bvh);
#elif defined USE_ROULETTE
            color += getRayColor(ray, objects, light_objects,bg_color, bvh);
#else
            color += getRayColor(ray, objects,light_objects, bg_color, 50, bvh);
#endif
          }

          {
            color /= fb.getSampleCount();
            fb.setPixelColor(j, i, color);
          }
        }

        counter++;
      }
    }

    fb.writeToFile(out_path, GAMMA);
  };

  void renderWithInfo(const std::string &out_path = "./output.png",
                      bool time_it = true, bool show_hitcount = true) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> duration;
    start = std::chrono::system_clock::now();
    render(out_path);
    end = std::chrono::system_clock::now();
    duration = end - start;

    if (time_it)
      std::cout << "Rendering time: " << duration.count() << " seconds"
                << std::endl;
    if (show_hitcount)
      std::cout << "Total hit count: " << hit_count << std::endl;
  }
};