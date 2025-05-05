#pragma once
#include "../base/material.hpp"
#include "../utils/matrix.hpp"

class SimpleDielectricDispersion : public Material {
public:
  /// ior = (ηR, ηG, ηB)
  SimpleDielectricDispersion(const gl::vec3 &ior) : ior(ior) {}

  bool scatter(const Ray &ray_in, HitRecord &rec, ScatterRecord &srec,
               float uc = gl::rand_num(), // coin-flip sample
               const gl::vec2 &u = {gl::rand_num(),
                                    gl::rand_num()}, // 2D microfacet sample
               TransportMode mode = TransportMode::Radiance,
               uint32_t flags = BxDFFlags::All) const override {}

private:
  gl::vec3 ior;
};
