// MFDielectricPDF_test.cpp
#include <cmath>
#include <gtest/gtest.h>
#include <random>

#include "../src/probs/mfDielectricPDF.hpp"
#include "../src/utils/orthoBasis.hpp"

using gl::vec2;
using gl::vec3;

using gl::vec3;
static constexpr float PI = 3.14159265358979323846f;

// Uniformly sample a direction on the unit sphere
static vec3 uniformSampleSphere(float u1, float u2) {
  float z = 1.0f - 2.0f * u1;
  float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
  float phi = 2.0f * PI * u2;
  return vec3(r * std::cos(phi), r * std::sin(phi), z);
}

// Uniformly sample a direction on the hemisphere z>0
static vec3 uniformSampleHemisphere(float u1, float u2) {
  float z = u1;
  float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
  float phi = 2.0f * PI * u2;
  return vec3(r * std::cos(phi), r * std::sin(phi), z);
}

TEST(MFDielectricPDF, FullSphereNormalization) {
  // set up PDF in Importance mode
  float eta = 1.5f;
  float alpha = 0.3f;
  TrowbridgeReitzDistribution distrib(alpha, alpha);
  OrthoBasis onb({0, 0, 1});
  vec3 wo(0, 0, 1);
  MFDielectricPDF pdf(distrib, onb, wo, eta, BxDFReflTransFlags::All,
                      TransportMode::Importance);

  std::mt19937 gen(42);
  std::uniform_real_distribution<float> dist(0, 1);

  const int N = 200000;
  double sum = 0;
  for (int i = 0; i < N; ++i) {
    vec3 wi = uniformSampleSphere(dist(gen), dist(gen));
    sum += pdf.at(wi);
  }
  double avg = sum / N;
  // ∫ p(w) dω ≈ avg * area(sphere) = avg * 4π
  double integral = avg * (4.0 * PI);
  EXPECT_NEAR(integral, 1.0, 0.05) << "Full‐sphere integral of PDF should be 1";
}

TEST(MFDielectricPDF, ReflectHemisphereIntegral) {
  float eta = 1.5f;
  float F0 = std::pow((eta - 1) / (eta + 1), 2);
  TrowbridgeReitzDistribution distrib(0.3f, 0.3f);
  OrthoBasis onb({0, 0, 1});
  vec3 wo(0, 0, 1);
  MFDielectricPDF pdf(distrib, onb, wo, eta, BxDFReflTransFlags::All,
                      TransportMode::Importance);

  std::mt19937 gen(123);
  std::uniform_real_distribution<float> dist(0, 1);

  const int N = 200000;
  double sumR = 0;
  for (int i = 0; i < N; ++i) {
    vec3 wi = uniformSampleHemisphere(dist(gen), dist(gen));
    sumR += pdf.at(wi);
  }
  double avgR = sumR / N;
  // ∫_hemisphere p(w) dω ≈ avgR * area(hemisphere)= avgR * 2π
  double Rint = avgR * (2.0 * PI);
  EXPECT_NEAR(Rint, F0, 0.02)
      << "Reflect‐hemisphere integral should match Fresnel R₀";
}

TEST(MFDielectricPDF, TransmitHemisphereIntegral) {
  float eta = 1.5f;
  float F0 = std::pow((eta - 1) / (eta + 1), 2);
  float T0 = 1.0f - F0;
  TrowbridgeReitzDistribution distrib(0.3f, 0.3f);
  OrthoBasis onb({0, 0, 1});
  vec3 wo(0, 0, 1);
  MFDielectricPDF pdf(distrib, onb, wo, eta, BxDFReflTransFlags::All,
                      TransportMode::Importance);

  std::mt19937 gen(456);
  std::uniform_real_distribution<float> dist(0, 1);

  const int N = 200000;
  double sumT = 0;
  for (int i = 0; i < N; ++i) {
    // sample lower hemisphere by flipping z
    vec3 w = uniformSampleHemisphere(dist(gen), dist(gen));
    vec3 wi = vec3(w.x(), w.y(), -w.z());
    sumT += pdf.at(wi);
  }
  double avgT = sumT / N;
  double Tint = avgT * (2.0 * PI);
  EXPECT_NEAR(Tint, T0, 0.02)
      << "Transmit‐hemisphere integral should match Fresnel T₀";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}