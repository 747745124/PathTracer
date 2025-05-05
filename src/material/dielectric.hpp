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

  bool scatter(const Ray &ray_in, HitRecord &rec, ScatterRecord &srec,
               float uc = gl::rand_num(), // coin-flip sample
               const gl::vec2 &u = {gl::rand_num(),
                                    gl::rand_num()}, // 2D microfacet sample
               TransportMode mode = TransportMode::Radiance,
               uint32_t flags = BxDFFlags::All) const override;

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered,
                    TransportMode mode = TransportMode::Radiance,
                    uint32_t flags = BxDFFlags::All) const override;

  gl::vec3 f(const gl::vec3 &wo_world, const gl::vec3 &wi_world,
             const HitRecord &rec,
             TransportMode mode = TransportMode::Radiance, ) const override;
};