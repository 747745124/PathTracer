#pragma once
#include "../utils/matrix.hpp"
#include "../utils/utility.hpp"

namespace gl {

inline vec3 uniformSampleHemiSphere() {
  float r1 = C_rand();
  float r2 = C_rand();
  float x = std::cos(2.f * M_PI * r1) * std::sqrt(r2 - r2 * r2);
  float y = std::sin(2.f * M_PI * r1) * std::sqrt(r2 - r2 * r2);
  float z = 1.f - r2;
  return vec3(x, y, z);
};

inline vec3 cosineSampleHemiSphere() {
  float r1 = C_rand();
  float r2 = C_rand();
  float x = std::cos(2.f * M_PI * r1) * std::sqrt(r2);
  float y = std::sin(2.f * M_PI * r1) * std::sqrt(r2);
  float z = std::sqrt(1 - r2);
  return vec3(x, y, z);
}

};