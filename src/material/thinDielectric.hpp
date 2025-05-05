#pragma once
#include "../base/material.hpp"
class ThinDielectric : public Material {
private:
  float eta;

public:
  ThinDielectric(float eta) : eta(eta){};

  bool scatter(const Ray &ray_in, HitRecord &rec, ScatterRecord &srec,
               float uc = gl::rand_num(), // coin-flip sample
               const gl::vec2 &u = {gl::rand_num(),
                                    gl::rand_num()}, // 2D microfacet sample
               TransportMode mode = TransportMode::Radiance,
               uint32_t flags = BxDFFlags::All) const override {
    using namespace gl;

    vec3 wo_world = -ray_in.getDirection().normalize();
    float absCosThetaO = fabs(dot(rec.normal, wo_world));

    float R = fresnelDielectric(absCosThetaO, eta), T = 1 - R;

    if (R < 1) {
      R += square(T) * R / (1 - square(R));
      T = 1 - R;
    }

    float pr = R, pt = T;
    if (pr == 0 && pt == 0)
      return false;

    if (uc < pr / (pr + pt)) {
      vec3 wi_world = pbrt::reflect(wo_world, rec.normal).normalize();
      srec.sampled_ray = Ray(rec.position, wi_world);
      srec.sampled_type = BxDFFlags::SpecularReflection;
      srec.pdf_ptr = nullptr;
      float cosThetaI = fabs(dot(rec.normal, wi_world));
      srec.attenuation = vec3(R / cosThetaI);
      srec.pdf_val = R;
    } else {
      vec3 wi_world = -wo_world.normalize();
      srec.sampled_ray = Ray(rec.position, wi_world);
      srec.sampled_type = BxDFFlags::SpecularTransmission;
      srec.pdf_ptr = nullptr;
      float cosThetaI = fabs(dot(rec.normal, wi_world));
      srec.attenuation = vec3(T / cosThetaI);
      srec.pdf_val = T;
    }
    return true;
  };

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered,
                    TransportMode mode = TransportMode::Radiance,
                    uint32_t flags = BxDFFlags::All) const override {
    return 0.f;
  }
};