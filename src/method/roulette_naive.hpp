#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../probs/hittablePDF.hpp"
#include "../probs/mixedPDF.hpp"
#include "../utils/bvh.hpp"
#include "../config.hpp"
inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            const ObjectList &light_objects, gl::vec3 bg_color,
                            std::shared_ptr<BVHNode> bvh = nullptr) {
  using namespace gl;

  float p = C_rand();
  float p_threshold = 0.4f;

  if (p < p_threshold)
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
                                            light_objects, bg_color, bvh)/(1-p_threshold);

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

    return (mat->emit(ray, hit_record) +
            albedo * getRayColor(out_ray, prims, light_objects, bg_color, bvh) *
                mat->scatter_pdf(ray, hit_record, out_ray) / pdf) /
           (1 - p_threshold);
  }

  return mat->emit(ray, hit_record) / (1 - p_threshold);
};
