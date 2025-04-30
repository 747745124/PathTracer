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

    using namespace gl;

    vec3 wo = -ray_in.getDirection().normalize();

    if (mfDistribution.effectivelySmooth()) {

      gl::vec3 wi = gl::pbrt::reflect(wo, rec.normal).normalize();
      if (dot(rec.normal, wi) <= 0)
        return false;

      srec.specular_ray = Ray(rec.position, wi);
      // Fresnel reflectance at this angle:
      float absCosThetaI = fabs(dot(rec.normal, wi));
      vec3 f = fresnelComplex(absCosThetaI, eta, k);
      srec.attenuation = f;
      srec.is_specular = true;
      srec.pdf_ptr = nullptr; // delta
      return true;
    }

    // --- rough (microfacet) case ---
    OrthoBasis basis(rec.normal);
    auto pdf_ptr = std::make_shared<MicrofacetPDF>(mfDistribution, basis, wo);

    // 2) sample an incoming direction `wi`
    vec3 wi = pdf_ptr->get();
    // 3) reject if it’s below the geometric normal
    if (dot(rec.normal, wi) <= 0)
      return false;

    srec.attenuation = f(wo, wi, rec);
    srec.is_specular = false;
    srec.pdf_ptr = pdf_ptr;
    return true;
  }

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered) const override {

    if (mfDistribution.effectivelySmooth()) {
      return 0.0f;
    }

    OrthoBasis basis(rec.normal);
    MicrofacetPDF mfPdf(mfDistribution, basis,
                        -ray_in.getDirection().normalize());
    return mfPdf.at(scattered.getDirection());
  };

  gl::vec3 f(const gl::vec3 &wo_world, const gl::vec3 &wi_world,
             const HitRecord &rec) const override {
    using namespace gl;

    OrthoBasis basis(rec.normal);
    vec3 wo_l = basis.toLocal(wo_world.normalize());
    vec3 wi_l = basis.toLocal(wi_world.normalize());
    // 2) below‐surface => zero
    if (wi_l.z() <= 0 || wo_l.z() <= 0) {
      return vec3(0.f);
    }
    // 3) handle delta case
    if (mfDistribution.effectivelySmooth()) {
      return vec3(0.f);
    }
    // 4) microfacet half-vector
    vec3 m_l = normalize(wo_l + wi_l);
    // 5) D, G, F
    float D = mfDistribution.D(m_l);
    float G = mfDistribution.G(wo_l, wi_l);
    vec3 F = fresnelComplex(fabs(dot(wo_l, m_l)), eta, k);
    // 6) f value
    vec3 f =
        D * F * G / (4.f * pbrt::absCosTheta(wo_l) * pbrt::absCosTheta(wi_l));

    return f;
  };

private:
  TrowbridgeReitzDistribution mfDistribution;
  // ior and absorption
  gl::vec3 eta, k;
};