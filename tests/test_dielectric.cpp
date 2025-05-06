// MFDielectricPDF_test.cpp
#include <cmath>
#include <gtest/gtest.h>
#include <random>

#include "../src/material/dielectric.hpp"
#include "../src/utils/orthoBasis.hpp"

using namespace gl;
namespace {

constexpr int kNumSamples = 256 * 1024; // enough for a stable estimate
constexpr float kTol = 0.03f;           // 3 % error tolerance

// Simple cosine‑weighted sampler for the reference integral
vec3 cosineHemisphere(float u1, float u2) {
  float r = std::sqrt(u1);
  float phi = 2.0f * M_PI * u2;
  return {r * std::cos(phi), r * std::sin(phi),
          std::sqrt(std::max(0.0f, 1.0f - u1))};
}

class MFDielectricEnergyTest : public ::testing::Test {
protected:
  MFDielectricEnergyTest()
      : dist_(0.2f, 0.2f),      // roughness α = 0.2
        bxdf_(1.5f, 0.2f, 0.2f) // eta, αx, αy  (rough dielectric)
  {
    rec_.normal = {0, 0, 1};
    rec_.position = {0, 0, 0};
  }

  TrowbridgeReitzDistribution dist_;
  MFDielectric bxdf_;
  HitRecord rec_;
};

// ---------------------------------------------------------------------------
// 1) Full energy should be ≈ 1.0 (R + T = 1 for loss‑free dielectric)
// ---------------------------------------------------------------------------
TEST_F(MFDielectricEnergyTest, EnergyConservation) {
  std::mt19937_64 rng(57);
  std::uniform_real_distribution<float> uni(0.0f, 1.0f);

  vec3 wo = normalize(vec3(0.3f, -0.2f, 0.93f)); // incident dir
  Ray woRay(rec_.position, -wo);

  float Lo = 0.0f;

  for (int i = 0; i < kNumSamples; ++i) {
    float uc = uni(rng);
    vec2 u = {uni(rng), uni(rng)};

    ScatterRecord sr;
    if (!bxdf_.scatter(woRay, rec_, sr, uc, u))
      continue;

    const vec3 wi = sr.sampled_ray.getDirection();
    const float f = sr.attenuation.length(); // monochrome
    const float cosI = fabs(dot(wi, rec_.normal));
    const float contrib = f * cosI / std::max(1e-6f, sr.pdf_val);
    Lo += contrib;
  }

  Lo /= kNumSamples;
  EXPECT_NEAR(Lo, 1.0f, kTol) << "Average reflected+transmitted energy too low";
}

// ---------------------------------------------------------------------------
// 2) For every *single* sample  f * cosθ / pdf  should be close to 1
//    (within a loose bound — floating noise & rough BRDF lobes)
// ---------------------------------------------------------------------------
TEST_F(MFDielectricEnergyTest, SingleSampleConsistency) {
  std::mt19937_64 rng(88);
  std::uniform_real_distribution<float> uni(0.0f, 1.0f);

  vec3 wo = normalize(vec3(-0.4f, 0.1f, 0.91f));
  Ray woRay(rec_.position, -wo);

  for (int i = 0; i < 4096; ++i) {
    float uc = uni(rng);
    vec2 u = {uni(rng), uni(rng)};

    ScatterRecord sr;
    if (!bxdf_.scatter(woRay, rec_, sr, uc, u))
      continue;

    const vec3 wi = sr.sampled_ray.getDirection();
    const float cosI = fabs(dot(wi, rec_.normal));
    const float brdf = sr.attenuation.length();
    const float test = brdf * cosI / std::max(1e-6f, sr.pdf_val);

    EXPECT_NEAR(test, 1.0f, 0.15f)
        << "Sample " << i << " not energy‑consistent; value = " << test;
  }
}

} // anonymous namespace