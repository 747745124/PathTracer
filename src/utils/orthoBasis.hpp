#pragma once
#include "./matrix.hpp"
#include "./utility.hpp"

class OrthoBasis {
private:
  std::array<gl::vec3, 3> axis;

public:
  gl::vec3 operator[](int i) { return axis[i]; };
  gl::vec3 u() { return axis[0]; };
  gl::vec3 v() { return axis[1]; };
  gl::vec3 w() { return axis[2]; };

  OrthoBasis(gl::vec3 n) {
    axis[2] = n.normalize();
    gl::vec3 a =
        (std::abs(n.x()) > 0.9) ? gl::vec3(0, 1, 0) : gl::vec3(1, 0, 0);
    axis[1] = gl::normalize(gl::cross(axis[2], a));
    axis[0] = gl::cross(axis[2], axis[1]);
  };

  gl::vec3 at(gl::vec3 v) {
    return v.x() * axis[0] + v.y() * axis[1] + v.z() * axis[2];
  };

  gl::vec3 at(float x, float y, float z) {
    return x * axis[0] + y * axis[1] + z * axis[2];
  }
};