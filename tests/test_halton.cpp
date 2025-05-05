#include "../src/sampler/sampler.hpp"
#include <gtest/gtest.h>

// Verify the first few values of the 1D Halton (base-2) sequence
TEST(HaltonSampler, Get1DFirstValues) {
  HaltonSampler sampler;
  sampler.reset();
  EXPECT_NEAR(sampler.get1D(), 0.5f, 1e-6f);
  EXPECT_NEAR(sampler.get1D(), 0.25f, 1e-6f);
  EXPECT_NEAR(sampler.get1D(), 0.75f, 1e-6f);
  EXPECT_NEAR(sampler.get1D(), 0.125f, 1e-6f);
}

// Verify the first few values of the 2D Halton (bases 2,3) sequence
TEST(HaltonSampler, Get2DFirstValues) {
  HaltonSampler sampler;
  sampler.reset();
  auto v1 = sampler.get2D();
  EXPECT_NEAR(v1.x(), 0.5f, 1e-6f);
  EXPECT_NEAR(v1.y(), 1.0f / 3.0f, 1e-6f);

  auto v2 = sampler.get2D();
  EXPECT_NEAR(v2.x(), 0.25f, 1e-6f);
  EXPECT_NEAR(v2.y(), 2.0f / 3.0f, 1e-6f);
}

// Test that reset() brings the sequence back to the start
TEST(HaltonSampler, ResetResetsSequence) {
  HaltonSampler sampler;
  sampler.get1D();
  sampler.get1D();
  sampler.reset();
  EXPECT_NEAR(sampler.get1D(), 0.5f, 1e-6f);
}

// Check basic statistics: mean ~0.5, variance ~1/12
TEST(HaltonSampler, BasicStatistics) {
  HaltonSampler sampler;
  sampler.reset();
  const int N = 1024;
  double sum = 0.0;
  double sumSq = 0.0;
  for (int i = 0; i < N; ++i) {
    double v = sampler.get1D();
    sum += v;
    sumSq += v * v;
  }
  double mean = sum / N;
  double var = sumSq / N - mean * mean;
  EXPECT_NEAR(mean, 0.5, 1e-2);
  EXPECT_NEAR(var, 1.0 / 12.0, 1e-2);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
