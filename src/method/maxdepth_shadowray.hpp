#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../utils/bvh.hpp"
#define LIGHT_SAMPLE_X 1
#define LIGHT_SAMPLE_Y 1
const int LIGHT_SAMPLE_NUM = LIGHT_SAMPLE_X * LIGHT_SAMPLE_Y;

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            gl::vec3 bg_color, const LightList &lights,
                            uint max_depth = 40,
                            std::shared_ptr<BVHNode> bvh = nullptr) {
  using namespace gl;

  if (max_depth == 0)
    return bg_color;

  HitRecord hit_record;
  bool is_hit = false;
  if (bvh == nullptr)
    is_hit = prims.intersect(ray, hit_record);
  else
    is_hit = bvh->intersect(ray, hit_record);

  if (!is_hit)
    return bg_color;

  Ray out_ray;
  vec3 albedo;
  float pdf = 0.f;
  auto mat = hit_record.material;
  if (mat->scatter(ray, hit_record, albedo, out_ray, pdf)) {
    float cos_theta = dot(hit_record.normal, out_ray.getDirection());
    cos_theta = std::max(cos_theta, 0.0f);
    gl::vec3 light_term = gl::vec3(0.f);
    auto offsets = getOffsets(LIGHT_SAMPLE_X, LIGHT_SAMPLE_Y);

    for (int i = 0; i < LIGHT_SAMPLE_NUM; i++) {
      try {
        auto light_sample = lights.uniform_get();
        auto light_dir =
            light_sample->get_sample(offsets[i][0], offsets[i][1]) -
            hit_record.position;

        Ray shadow_ray(hit_record.position, light_dir.normalize());
        HitRecord shadow_hit_record;
        bool is_shadow_hit = false;
        if (bvh == nullptr)
          is_shadow_hit = prims.intersect(shadow_ray, shadow_hit_record, 0.001f,
                                          light_dir.length() - 0.001f);
        else
          is_shadow_hit = bvh->intersect(shadow_ray, shadow_hit_record, 0.001f,
                                         light_dir.length() - 0.001f);

        auto NoL =
            std::max(dot(hit_record.normal, shadow_ray.getDirection()), 0.f);
        auto G = std::max(NoL, 0.0f) * cos_theta / (dot(light_dir, light_dir));
        auto BRDF =
            albedo * mat->scatter_pdf(ray, hit_record, out_ray) / cos_theta;
        auto V = is_shadow_hit ? 0.f : 1.f;
        auto direct_term = BRDF * light_sample->intensity *
                           light_sample->color * light_sample->get_area() * G *
                           V;

        light_term += direct_term;
      } catch (...) {
        continue;
      }
    }

    HitRecord next_hit_record;
    bool is_next_hit = false;

    Ray temp_ray = out_ray;
    float temp_pdf = 0.f;
    gl::vec3 temp_albedo;

    if (bvh == nullptr)
      is_next_hit = prims.intersect(out_ray, next_hit_record);
    else
      is_next_hit = bvh->intersect(out_ray, next_hit_record);

    // next-event estimation,avoid double counting
    if (is_next_hit &&
        (!next_hit_record.material->scatter(out_ray, next_hit_record,
                                            temp_albedo, temp_ray, temp_pdf)))
      return 0.f;

    // direct light sampling + indirect light sampling
    return mat->emit(hit_record) + light_term / LIGHT_SAMPLE_NUM +
           albedo *
               getRayColor(out_ray, prims, bg_color, lights, max_depth - 1,
                           bvh) *
               mat->scatter_pdf(ray, hit_record, out_ray) / pdf;
  }

  return mat->emit(hit_record);
};