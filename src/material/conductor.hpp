#pragma once
#include "../base/material.hpp"
#include "../probs/mfPDF.hpp"
class Conductor : public Material {
public:
  Conductor(const gl::vec3 &eta, const gl::vec3 &k, float alpha_x,
            float alpha_y)
      : eta(eta), k(k), mfDistribution(alpha_x, alpha_y) {}

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {

    OrthoBasis basis(rec.normal);
    auto pdf_ptr = std::make_shared<MicrofacetPDF>(mfDistribution, basis,
                                                   ray_in.getDirection());

    if (mfDistribution.effectivelySmooth()) {
      gl::vec3 inDir = ray_in.getDirection().normalize();
      gl::vec3 R = reflect(inDir, rec.normal);

      srec.specular_ray = Ray(rec.position, R);

      // Fresnel reflectance at this angle:
      float cosThetaI = fabs(dot(rec.normal, inDir));

      srec.attenuation = fresnelComplex(cosThetaI, eta, k);
      srec.is_specular = true;
      srec.pdf_ptr = nullptr; // delta
      return true;
    }

    return false;

    // if (gl::dot(wo_world, rec.normal) <= 0.0f)
    //   return false;

    // // Compute half-vector in world space
    // gl::vec3 m_world = (wi_local + wo_world).normalize();

    // // Fresnel term (using conductor IOR k, eta)
    // float cosThetaI = std::abs(dot(wi_flip, m_world));
    // gl::vec3 F = fresnelConductor(cosThetaI, eta, k);

    // float D = mfDistribution.D(m_world);
    // float G = mfDistribution.G(wi_flip, wo_world);
    // float cosI = std::abs(dot(rec.normal, wi_world));
    // float cosO = std::abs(dot(rec.normal, wo_world));
  }

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered) const override {
    if (mfDistribution.effectivelySmooth()) {
      return 0.0f;
    }

    return 0.0f;
  };

private:
  TrowbridgeReitzDistribution mfDistribution;
  // ior and absorption
  gl::vec3 eta, k;
};