#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../probs/hittablePDF.hpp"
#include "../probs/mixedPDF.hpp"
#include "../utils/bvh.hpp"
#define LIGHT_SAMPLE_X 13
#define LIGHT_SAMPLE_Y 13
const int LIGHT_SAMPLE_NUM = LIGHT_SAMPLE_X * LIGHT_SAMPLE_Y;

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            const ObjectList &light_objects, gl::vec3 bg_color,
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
  ScatterRecord srec;
  float pdf = 0.f;
  auto mat = hit_record.material;

  if (mat->scatter(ray, hit_record, srec)) {

    // if it's specular, just reflect
    if (srec.is_specular)
      return srec.attenuation * getRayColor(srec.specular_ray, prims,
                                            light_objects, bg_color,
                                            max_depth - 1, bvh);
    auto pdfs = std::vector<std::shared_ptr<PDF>>();
    for (int i = 0; i < LIGHT_SAMPLE_NUM; i++) {
      auto light = light_objects.uniform_get();
      auto light_pdf =
          std::make_shared<HittablePDF>(hit_record.position, light);
      pdfs.push_back(light_pdf);
    }

    if (srec.pdf_ptr != nullptr)
      pdfs.push_back(srec.pdf_ptr);
    auto mix_pdf = MixedPDF(pdfs);

    out_ray = Ray(hit_record.position, mix_pdf.get().normalize());
    albedo = srec.attenuation;
    pdf = mix_pdf.at(out_ray.getDirection().normalize());

    return mat->emit(ray, hit_record) +
           albedo *
               getRayColor(out_ray, prims, light_objects, bg_color,
                           max_depth - 1, bvh) *
               mat->scatter_pdf(ray, hit_record, out_ray) / pdf;
  }

  return mat->emit(ray, hit_record);
};
