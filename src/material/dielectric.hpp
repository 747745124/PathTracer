#pragma once
#include "../base/material.hpp"
#include "../probs/mfPDF.hpp"

class MFDielectric : public Material {
private:
  float eta;
  TrowbridgeReitzDistribution mfDistrib;

public:
  MFDielectric(float eta, TrowbridgeReitzDistribution mfDistrib)
      : eta(eta), mfDistrib(mfDistrib){};

  MFDielectric(float eta, float alpha_x, float alpha_y)
      : eta(eta), mfDistrib(alpha_x, alpha_y){};

  bool
  scatter(const Ray &ray_in, HitRecord &rec, ScatterRecord &srec,
          float uc = gl::rand_num(), // coin-flip sample
          const gl::vec2 &u = {gl::rand_num(),
                               gl::rand_num()}, // 2D microfacet sample
          TransportMode mode = TransportMode::Radiance,
          BxDFReflTransFlags flags = BxDFReflTransFlags::All) const override {
    if (eta == 1 || mfDistrib.effectivelySmooth()) {
      using namespace gl;

      vec3 wo_world = -ray_in.getDirection().normalize();
      float cosThetaO = fabs(dot(rec.normal, wo_world));

      float R = gl::fresnelDielectric(cosThetaO, eta), T = 1 - R;
      float pr = R, pt = T;
      if (!(flags & BxDFReflTransFlags::Reflection))
        pr = 0;
      if (!(flags & BxDFReflTransFlags::Transmission))
        pt = 0;
      if (pr == 0 && pt == 0)
        return false;

      if (uc < pr / (pr + pt)) {
        vec3 wi_world = pbrt::reflect(wo_world, rec.normal).normalize();
        srec.sampled_ray = Ray(rec.position, wi_world);
        srec.sampled_type = BxDFFlags::SpecularReflection;
        srec.pdf_ptr = nullptr;
        srec.attenuation = 1.0f;
        srec.pdf_val = R;
        return true;
      } else {
        vec3 wi_world;
        float etap;
        // TIR
        bool valid = pbrt::refract(wo_world, rec.normal, eta, etap, wi_world);
        if (!valid)
          return false;

        srec.sampled_ray = Ray(rec.position, wi_world);
        srec.sampled_type = BxDFFlags::SpecularTransmission;
        srec.pdf_ptr = nullptr;
        srec.pdf_val = T;
        if (mode == TransportMode::Radiance)
          srec.attenuation = 1.0f / square(etap);
        else
          srec.attenuation = 1.0f;
        return true;
      }
      // rough branch
    }

    return false;
  };

  float scatter_pdf(
      const Ray &ray_in, const HitRecord &rec, const Ray &scattered,
      TransportMode mode = TransportMode::Radiance,
      BxDFReflTransFlags flags = BxDFReflTransFlags::All) const override {
    return 0.f;
  }

  // required for non-delta
  gl::vec3 f(const gl::vec3 &wo_world, const gl::vec3 &wi_world,
             const HitRecord &rec,
             TransportMode mode = TransportMode::Radiance) const override {
    return gl::vec3(0.f);
  };
};