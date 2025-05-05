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
private:
  uint32_t sampleIndex = 0;

public:
  HaltonSampler() = default;
  // Reset the sequence (e.g. at the start of each pixel)
  void reset() { sampleIndex = 0; }

  // 1D Halton in base 2
  float get1D() override { return gl::radicalInverse(0, ++sampleIndex); }

  // 2D Halton: x in base 2, y in base 3
  gl::vec2 get2D() override {
    uint32_t i = ++sampleIndex;
    return {gl::radicalInverse(0, i), gl::radicalInverse(1, i)};
  }
};

extern thread_local HaltonSampler halton_sampler;
