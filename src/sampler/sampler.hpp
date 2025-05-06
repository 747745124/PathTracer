#pragma once
#include "../utils/utility.hpp"

class Sampler {
public:
  Sampler() = default;
  virtual ~Sampler() = default;

  virtual float get1D() = 0;
  virtual gl::vec2 get2D() = 0;
};

class HaltonSampler : public Sampler {
  uint32_t sampleIndex = 0; // which point in the sequence
  int dimension = 0;        // which coordinate of that point

public:
  // required to call before each sample ray
  void startSample() {
    sampleIndex++;
    dimension = 0;
  }

  // Returns the next 1D Halton component for this sample
  float get1D() override {
    float v = gl::radicalInverse(dimension, sampleIndex);
    dimension++;
    return v;
  }

  // Returns two consecutive components for this sample
  gl::vec2 get2D() override {
    float x = gl::radicalInverse(dimension, sampleIndex);
    float y = gl::radicalInverse(dimension + 1, sampleIndex);
    dimension += 2;
    return {x, y};
  }
};

extern thread_local HaltonSampler halton_sampler;
