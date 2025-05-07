#pragma once
#include "../utils/utility.hpp"
#include "./digitPerm.hpp"

enum class RandomStrategy { None, PermuteDigits, Owen };

class Sampler {
public:
  Sampler() = default;
  virtual ~Sampler() = default;

  virtual float get1D() = 0;
  virtual gl::vec2 get2D() = 0;
};

class HaltonSampler : public Sampler {
public:
  explicit HaltonSampler(std::uint32_t seed = 0,
                         RandomStrategy strat = RandomStrategy::Owen)
      : strategy(strat), seed_(seed) {
    if (strategy == RandomStrategy::PermuteDigits) {
      // Precompute the digit permutations once.
      digitPerms_ = std::make_shared<std::vector<DigitPermutation>>(
          gl::computeRadicalInversePermutations(seed_));
    }
  }

  // Call before starting a new pixel / ray / path
  void startSample() {
    ++sampleIndex_;
    dimension_ = 0;
  }

  // One‑dimensional component
  float get1D() override { return SampleDimension(dimension_++); }

  // Two consecutive components
  gl::vec2 get2D() override {
    float x = SampleDimension(dimension_++);
    float y = SampleDimension(dimension_++);
    return {x, y};
  }

private:
  // ---------------------------------------------------------------------
  // Core helper: fetches the radical‑inverse value for the given dimension
  // according to the current randomisation strategy.
  // ---------------------------------------------------------------------
  float SampleDimension(int dim) const {
    switch (strategy) {
    case RandomStrategy::None:
      return static_cast<float>(gl::radicalInverse(dim, sampleIndex_));

    case RandomStrategy::PermuteDigits:
      return static_cast<float>(
          gl::scrambledRadicalInverse(dim, sampleIndex_, (*digitPerms_)[dim]));

    case RandomStrategy::Owen:
    default:
      return static_cast<float>(gl::owenScrambledRadicalInverse(
          dim, sampleIndex_,
          gl::pbrt::mixBits(1u + (static_cast<std::uint32_t>(dim) << 4))));
    }
  }

  // Sampler state --------------------------------------------------------
  std::uint32_t sampleIndex_ = 0; // which point in the sequence
  int dimension_ = 0;             // which coordinate of that point

  // Configuration --------------------------------------------------------
  RandomStrategy strategy = RandomStrategy::Owen;
  std::uint32_t seed_ = 0;
  std::shared_ptr<std::vector<DigitPermutation>> digitPerms_;
};

extern thread_local HaltonSampler halton_sampler;
