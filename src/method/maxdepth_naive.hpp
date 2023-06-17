#pragma once
#include "../base/lightList.hpp"
#include "../base/objectList.hpp"
#include "../utils/bvh.hpp"
#include "../probs/hittablePDF.hpp"

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
  float pdf = 0.f;
  auto mat = hit_record.material;
  if (mat->scatter(ray, hit_record, albedo, out_ray, pdf)) {
    // auto light = light_objects.uniform_get();

    auto light = std::make_shared<AARectangle<Axis::Y>>(554, 213, 343, 227, 332, std::make_shared<DiffuseEmitter>(gl::vec3(1.0f),15));
    HittablePDF light_pdf(hit_record.position, light);
    out_ray = Ray(hit_record.position, light_pdf.get());
    pdf = light_pdf.at(out_ray.getDirection());

    return mat->emit(hit_record) +
           albedo * getRayColor(out_ray, prims,light_objects,bg_color, max_depth - 1, bvh) *
               mat->scatter_pdf(ray, hit_record, out_ray) / pdf;
  }

  return mat->emit(hit_record);
};
