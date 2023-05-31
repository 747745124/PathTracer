#pragma once
#include "../base/camera.hpp"
#include "../base/material.hpp"
#include "../base/objectList.hpp"
#include "../base/primitive.hpp"
#include "../method/maxdepth_tracer.hpp"
#include "../method/pathtracing.hpp"
#include "../utils/bvh.hpp"
#include "../utils/timeit.hpp"

extern int hit_count;

struct SceneInfo {
  std::shared_ptr<PerspectiveCamera> camera = nullptr;
  std::shared_ptr<BVHNode> bvh = nullptr;
  ObjectList objects;
  gl::vec3 bg_color = gl::vec3(0.7, 0.8, 1.0);
  bool use_bvh = true;
  uint _width = 800;
  uint _height = 800;
  uint spp_x = 2;
  uint spp_y = 2;
  float GAMMA = 1.0f;

  SceneInfo() = default;

  void render(const std::string &out_path = "../output.png") {

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
            color += getRayColor(ray, objects, bg_color, bvh);
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

  void renderWithInfo(const std::string &out_path = "../output.png",
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