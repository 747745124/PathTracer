#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../config.hpp"
#include "../probs/hittablePDF.hpp"
#include "../probs/mixedPDF.hpp"
#include "../sampler/sampler.hpp"
#include "../utils/bvh.hpp"
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

  ScatterRecord srec;
  auto mat = hit_record.material;
  float uc = halton_sampler.get1D();
  vec2 u = halton_sampler.get2D();
  uc = rand_num();
  u = vec2(rand_num(), rand_num());
  if (mat->scatter(ray, hit_record, srec, uc, u, MODE)) {

    // ray splitting, if it has both specular
    bool hasRefl = srec.is_specular_reflection();
    bool hasTran = srec.is_specular_transmission();

    if (hasRefl && hasTran) {
      float R = srec.pdf_val;
      float T = 1.0f - R;
      vec3 wo = -ray.getDirection().normalize();
      vec3 n = hit_record.normal;
      vec3 sum(0.0f);

      // branch 0 = reflect, 1 = transmit
      for (int b = 0; b < 2; ++b) {
        bool doRefl = (b == 0);
        float w = doRefl ? R : T;
        vec3 wi = doRefl ? pbrt::reflect(wo, n) : -wo;
        Ray next = Ray(hit_record.position, wi);
        sum +=
            w * getRayColor(next, prims, bg_color, lights, max_depth - 1, bvh);
      }

      return mat->emit(ray, hit_record) + sum;
    }

    if (hasRefl || hasTran) {
      return mat->emit(ray, hit_record) +
             srec.attenuation * getRayColor(srec.sampled_ray, prims, bg_color,
                                            lights, max_depth - 1, bvh);
    }

    auto pdf_ptr = srec.pdf_ptr
                       ? srec.pdf_ptr
                       : std::make_shared<CosinePDF>(hit_record.normal);

    auto wo_world = -ray.getDirection().normalize();
    auto wi_world = srec.sampled_ray.getDirection().normalize();
    float cos_theta = dot(hit_record.normal, wi_world);
    cos_theta = std::max(cos_theta, 0.0f);
    auto pdf_val = srec.pdf_val;

    // note that below are sample from light,i.e. DI
    gl::vec3 light_term = gl::vec3(0.f);
    auto offsets = getOffsets(LIGHT_SAMPLE_X, LIGHT_SAMPLE_Y);
    for (int i = 0; i < LIGHT_SAMPLE_NUM; i++) {
      auto light_sample = lights.uniform_get();
      auto light_p = light_sample->get_sample(offsets[i][0], offsets[i][1]);
      auto light_dir = light_p - hit_record.position;
      auto light_normal = light_sample->get_normal_at(light_p);

      Ray shadow_ray(hit_record.position, light_dir.normalize());
      HitRecord shadow_hit_record;

      bool is_shadow_hit = false;

      // note that the tmin and tmax are set to exclude the light
      if (bvh == nullptr)
        is_shadow_hit = prims.intersect(shadow_ray, shadow_hit_record, 0.001f,
                                        light_dir.length() - 0.001f);
      else
        is_shadow_hit = bvh->intersect(shadow_ray, shadow_hit_record, 0.001f,
                                       light_dir.length() - 0.001f);

      auto light_pdf = 1.f / (light_sample->get_area() * lights.size());
      // cos wi
      auto NoI =
          std::max(dot(hit_record.normal, shadow_ray.getDirection()), 0.f);
      // cos wl
      auto NoL = std::max(dot(light_normal, -shadow_ray.getDirection()), 0.f);

      auto G = NoL * NoI / (dot(light_dir, light_dir));
      auto hit_light = false;
      auto V = is_shadow_hit ? 0.0f : 1.0f;

      auto wi_world = shadow_ray.getDirection().normalize();
      auto f = mat->f(wo_world, wi_world, hit_record, MODE);

      auto direct_term =
          f * light_sample->intensity * light_sample->color * G * V / light_pdf;
      light_term += direct_term;
    }

    HitRecord next_hit_record;
    ScatterRecord next_scatter_record;
    bool is_next_hit = false;

    if (bvh == nullptr)
      is_next_hit =
          prims.intersect(Ray(hit_record.position, wi_world), next_hit_record);
    else
      is_next_hit =
          bvh->intersect(Ray(hit_record.position, wi_world), next_hit_record);

    // next-event estimation, avoid double counting
    if (is_next_hit && next_hit_record.material->is_emitter())
      return mat->emit(ray, hit_record) + light_term / LIGHT_SAMPLE_NUM;

    auto f = srec.attenuation;
    // direct light sampling + indirect light
    return mat->emit(ray, hit_record) + light_term / LIGHT_SAMPLE_NUM +
           f *
               getRayColor(Ray(hit_record.position, wi_world), prims, bg_color,
                           lights, max_depth - 1, bvh) *
               cos_theta / pdf_val;
  }

  return mat->emit(ray, hit_record);
};