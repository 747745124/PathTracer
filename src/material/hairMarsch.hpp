#pragma once
#include "../base/material.hpp"
class HairMarschner : public Material {
public:
  // assume alpha given in radian
  HairMarschner(const gl::vec3 &sigma_a, float h, float eta, float beta_m,
                float beta_n, float alpha)
      : sigma_a(sigma_a), h(h), eta(eta), beta_m(beta_m), beta_n(beta_n) {

    // initialize v mapping
    this->v[0] = gl::square(0.726f * beta_m + 0.812f * gl::square(beta_m) +
                            3.7f * std::pow(beta_m, 20.f));
    this->v[1] = .25 * v[0];
    this->v[2] = 4 * v[0];
    for (int p = 3; p <= pMax; ++p)
      this->v[p] = this->v[2];

    // intialize s mapping
    static const float SqrtPiOver8 = 0.626657069f;
    s = SqrtPiOver8 * (0.265f * beta_n + 1.194f * gl::square(beta_n) +
                       5.372f * std::pow(beta_n, 22.f));

    // initialize sin2kAlpha and cos2kAlpha
    sin2kAlpha[0] = std::sin(alpha);
    cos2kAlpha[0] = gl::safeSqrt(1 - gl::square(sin2kAlpha[0]));
    for (int i = 1; i < pMax; ++i) {
      sin2kAlpha[i] = 2 * cos2kAlpha[i - 1] * sin2kAlpha[i - 1];
      cos2kAlpha[i] =
          gl::square(cos2kAlpha[i - 1]) - gl::square(sin2kAlpha[i - 1]);
    }
  }

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {
    // gl::vec3 wi = ray_in.getDirection();
    // OrthoBasis basis(rec.hair_tangent);
    // auto pdf_ptr = std::make_shared<CosinePDF>(rec.normal);
    // auto wo = pdf_ptr->get();

    // const gl::vec3 wo_local = basis.at(wo);
    // const gl::vec3 wi_local = basis.at(wi);

    // gl::vec3 bsdf = evalMarschner(wi_local, wo_local);
    // srec.attenuation = bsdf;
    // srec.is_specular = false;
    // srec.pdf_ptr = pdf_ptr;
    // return true;
  }

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered) const override {
    // TODO
  }

private:
  static constexpr int pMax = 3;
  // initialized with passed parameters
  float h, eta;
  gl::vec3 sigma_a;
  float beta_m, beta_n;
  // will be initialize in constructor
  float v[pMax + 1];
  float s;
  float sin2kAlpha[pMax], cos2kAlpha[pMax];

  gl::vec3 evalMarschner(const gl::vec3 &wi, const gl::vec3 &wo) const;
  // longitudinal scattering, complex, ignore the details
  float Mp(float cosTheta_i, float cosTheta_o, float sinTheta_i,
           float sinTheta_o, float v) const {
    float a = cosTheta_i * cosTheta_o / v, b = sinTheta_i * sinTheta_o / v;
    float mp =
        (v <= .1) ? (gl::fastExp(gl::logI0f(a) - b - 1 / v + 0.6931f +
                                 std::log(1 / (2 * v))))
                  : (gl::fastExp(-b) * gl::I0f(a)) / (std::sinh(1 / v) * 2 * v);
    return mp;
  }

  float Phi(int p, float gamma_o, float gamma_t) const {
    return 2 * p * gamma_t - 2 * gamma_o + p * M_PI;
  }
  // Absorption
  std::array<gl::vec3, pMax + 1> Ap(float cosTheta_o, float eta, float h,
                                    gl::vec3 T) const {
    using namespace gl;
    std::array<gl::vec3, pMax + 1> ap;
    float cosGamma_o = safeSqrt(1 - square(h));
    float cosTheta = cosTheta_o * cosGamma_o;
    float f = fresnelDielectric(cosTheta, eta);
    ap[0] = vec3(f);
    ap[1] = square(1 - f) * T;
    for (int p = 2; p < pMax; ++p)
      ap[p] = ap[p - 1] * T * f;

    if (!(vec3(1.f) - T * f).near_zero())
      ap[pMax] = ap[pMax - 1] * f * T / (vec3(1.f) - T * f);

    return ap;
  }

  // azimuthal scattering
  float Np(float phi, int p, float s, float gamma_o, float gamma_t) const {
    float dphi = phi - Phi(p, gamma_o, gamma_t);
    while (dphi > M_PI)
      dphi -= 2 * M_PI;
    while (dphi < -M_PI)
      dphi += 2 * M_PI;

    return gl::trimmedLogistic(dphi, s, -M_PI, M_PI);
  }
};
