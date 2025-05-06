#pragma once
#include "../base/camera.hpp"
#include "../base/framebuffer.hpp"
#include "../base/material.hpp"
#include "../base/medium.hpp"
#include "../base/objectList.hpp"
#include "../base/primitive.hpp"
#include "../config.hpp"
#include "../mesh_io/meshLoader.hpp"
#include "../primitives/box.hpp"
#include "../primitives/curve.hpp"
#include "../utils/bvh.hpp"
#include "../utils/objectTransform.hpp"
#include "../utils/timeit.hpp"

#ifdef USE_ANALYTICAL_ILLUMIN
#include "../render_method/analytical_illumin.hpp"
#elif defined USE_MAXDEPTH_MIS
#include "../render_method/maxdepth_mis.hpp"
#elif defined USE_MAXDEPTH_NAIVE
#include "../render_method/maxdepth_naive.hpp"
#elif defined USE_MAXDEPTH_NEE
#include "../render_method/maxdepth_nee.hpp"
#elif defined USE_MAXDEPTH_RESERVOIR
#include "../render_method/max_depth_reservoir_di.hpp"
#elif defined USE_ROULETTE_NAIVE
#include "../render_method/roulette_naive.hpp"
#endif

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
  float _gamma = 1.0f;
  LightList lights = {std::make_shared<QuadLight>(light_info)};
  SceneInfo() = default;

  void render(const std::string &out_path = "./output.png",
              bool show_progress = true) {

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

#ifdef OVERRIDE_LOCAL_RENDER_VAL
    _width = WIDTH;
    _height = HEIGHT;
    spp_x = SPP_X;
    spp_y = SPP_Y;
    _gamma = GAMMA;
    bg_color = BG_COLOR;
    use_bvh = useBVH;
#endif

    if (use_bvh)
      bvh = make_shared<BVHNode>(objects);

    FrameBuffer fb(_width, _height, spp_x, spp_y);
    auto offsets = fb.getOffsets();
    uint counter = 0;

    light_objects = ObjectList(lights);

#pragma omp parallel for
    {
      for (int i = 0; i < _width; i++) {

        if (show_progress)
          std::cout << "Now scanning " << (float(counter) / _width) * 100.f
                    << " %" << std::endl;

        for (int j = 0; j < _height; j++) {
          auto color = vec3(0.0);

          // per sample
          for (int k = 0; k < fb.getSampleCount(); k++) {

            // per sample, reset sampler
            halton_sampler.startSample();
            auto sample_color = vec3(0.0);
            vec2 uv = (vec2(i, j) + offsets[k]) / vec2(_width, _height);
            Ray ray = camera->generateRay(uv.u(), uv.v());

#ifdef USE_ANALYTICAL_ILLUMIN
            color += getRayColor(ray, objects, bg_color, lights, bvh);
#elif defined USE_MAXDEPTH_NEE
            color +=
                getRayColor(ray, objects, bg_color, lights, MAX_RAY_DEPTH, bvh);
#elif defined USE_ROULETTE_NAIVE
            color += getRayColor(ray, objects, light_objects, bg_color,
                                 MAX_RAY_DEPTH, bvh);
#elif defined USE_RESERVOIR
            color +=
                getRayColor(ray, objects, bg_color, lights, MAX_RAY_DEPTH, bvh);
#elif defined USE_MAXDEPTH_NAIVE
            color += getRayColor(ray, objects, light_objects, bg_color,
                                 MAX_RAY_DEPTH, bvh);
#elif defined USE_MAXDEPTH_MIS
            color +=
                getRayColor(ray, objects, bg_color, lights, MAX_RAY_DEPTH, bvh);
#else
            std::cout << "No method selected!" << std::endl;
            std::runtime_error(
                "No method selected! Please define a method in the config.hpp");
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
                      bool time_it = true, bool show_hitcount = true,
                      bool show_progress = true) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> duration;
    start = std::chrono::system_clock::now();
    render(out_path, show_progress);
    end = std::chrono::system_clock::now();
    duration = end - start;

    if (time_it)
      std::cout << "Rendering time: " << duration.count() << " seconds"
                << std::endl;
    if (show_hitcount)
      std::cout << "Total hit count: " << gl::hit_count << std::endl;
  }
};