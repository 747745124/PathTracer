#pragma once
#include "../base/material.hpp"
#include "../utils/matrix.hpp"

class SimpleDielectricDispersion : public Material {
public:
  /// ior = (ηR, ηG, ηB)
  SimpleDielectricDispersion(const gl::vec3 &ior) : ior(ior) {}

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {}

private:
  gl::vec3 ior;
};
