#pragma once
#include "../utils/matrix.hpp"
#include "../utils/orthoBasis.hpp"
#include "../utils/utility.hpp"
#include "./random.hpp"

class PDF {
public:
  virtual float at(const gl::vec3 &direction) const = 0;
  virtual gl::vec3 get() const = 0;
  virtual ~PDF() = default;
};

class CosinePDF : public PDF {
  OrthoBasis onb;

public:
  CosinePDF(const gl::vec3 &direction) : onb(direction) {}
  float at(const gl::vec3 &direction) const override {
    float cosine = gl::dot(direction.normalize(), onb.w());
    return std::max(cosine, 0.f) / M_PI;
  }

  gl::vec3 get() const override { return onb.at(gl::cosineSampleHemiSphere()); }
};

