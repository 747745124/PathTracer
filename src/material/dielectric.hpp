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

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override;

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered) const override;

  gl::vec3 f(const gl::vec3 &wo_world, const gl::vec3 &wi_world,
             const HitRecord &rec) const override;
};