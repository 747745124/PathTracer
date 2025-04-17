#pragma once
#include "../utils/scene_io.hpp"
#include "../utils/utility.hpp"
#include "./object3D.hpp"
#include "./primitive.hpp"
#include <iostream>
#include <memory>
struct PolyLightInfo {
  std::vector<gl::vec3> vertices;
  gl::vec3 color;
  float intensity;
};

class Light : public Object3D {
public:
  Light() {
    position = gl::vec3(0.f, 0.f, 0.0f);
    color = gl::vec3(1.0f, 1.0f, 1.0f);
    intensity = 1.0f;
  };

  Light(const gl::vec3 &position, const gl::vec3 &color, float intensity) {
    this->color = {1.0f, 1.0f, 1.0f};
    this->intensity = 1.0f;
  }

  virtual gl::vec3 uniform_sample() const = 0;
  virtual gl::vec3 get_sample(float u, float v) const = 0;
  virtual float get_area() const { return 1.f; }
  virtual gl::vec3 get_normal_at(const gl::vec3 &p) const = 0;
  
  ~Light() = default;

  float intensity = 1.0f;
  gl::vec3 color = {1.0f, 1.0f, 1.0f};
  LightType type = LightType::POINT_LIGHT;
};

class PointLight : public Light {
public:
  PointLight(const gl::vec3 &position, const gl::vec3 &color,
             float intensity = 1.0f) {
    this->type = LightType::POINT_LIGHT;
    this->position = position;
    this->color = color;
    this->intensity = intensity;
  };

  ~PointLight() = default;
};

class DirectionalLight : public Light {
public:
  DirectionalLight(const gl::vec3 &color, const gl::vec3 &direction,
                   float intensity) {
    this->color = color;
    this->defaultFront = direction;
    this->intensity = intensity;
    this->position = gl::vec3(0.0f, 0.0f, 0.0f);
    this->type = LightType::DIRECTIONAL_LIGHT;
  };

  ~DirectionalLight() = default;
};

class SpotLight : public Light {
public:
  SpotLight(const gl::vec3 &direction, const gl::vec3 &position,
            const gl::vec3 &color, float intensity,
            float cutoff_angle = gl::to_radian(180.0f),
            float dropoff_rate = 1.0f) {
    this->defaultFront = direction;
    this->position = position;
    this->color = color;
    this->intensity = intensity;
    this->cutoff_angle = cutoff_angle;
    this->dropoff_rate = dropoff_rate;
    this->type = LightType::SPOT_LIGHT;
  };

  ~SpotLight() = default;
  float cutoff_angle = gl::to_radian(180.0f);
  float dropoff_rate = 1.0f;
};

class QuadLight : public Light {
public:
  std::array<gl::vec3, 4> vertices;
  QuadLight(const PolyLightInfo &info) {
    this->type = LightType::QUAD_LIGHT;
    assert(info.vertices.size() == 4);
    for (int i = 0; i < 4; i++) {
      this->vertices[i] = info.vertices[i];
    }
    this->color = info.color;
    this->intensity = info.intensity;
  }

  QuadLight(std::shared_ptr<AARectangle<Axis::X>> quad, gl::vec3 color,
            float intensity) {
    this->type = LightType::QUAD_LIGHT;
    gl::vec3 v0 = {quad->_k, quad->_d0_min, quad->_d1_min};
    gl::vec3 v1 = {quad->_k, quad->_d0_max, quad->_d1_min};
    gl::vec3 v2 = {quad->_k, quad->_d0_max, quad->_d1_max};
    gl::vec3 v3 = {quad->_k, quad->_d0_min, quad->_d1_max};
    this->vertices = {v0, v1, v2, v3};
    this->color = color;
    this->intensity = intensity;
  };

  QuadLight(std::shared_ptr<AARectangle<Axis::Y>> quad, gl::vec3 color,
            float intensity) {
    this->type = LightType::QUAD_LIGHT;
    gl::vec3 v0 = {quad->_d0_min, quad->_k, quad->_d1_min};
    gl::vec3 v1 = {quad->_d0_max, quad->_k, quad->_d1_min};
    gl::vec3 v2 = {quad->_d0_max, quad->_k, quad->_d1_max};
    gl::vec3 v3 = {quad->_d0_min, quad->_k, quad->_d1_max};
    this->vertices = {v0, v1, v2, v3};
    this->color = color;
    this->intensity = intensity;
  };

  QuadLight(std::shared_ptr<AARectangle<Axis::Z>> quad, gl::vec3 color,
            float intensity) {
    this->type = LightType::QUAD_LIGHT;
    gl::vec3 v0 = {quad->_d0_min, quad->_d1_min, quad->_k};
    gl::vec3 v1 = {quad->_d0_max, quad->_d1_min, quad->_k};
    gl::vec3 v2 = {quad->_d0_max, quad->_d1_max, quad->_k};
    gl::vec3 v3 = {quad->_d0_min, quad->_d1_max, quad->_k};
    this->vertices = {v0, v1, v2, v3};
    this->color = color;
    this->intensity = intensity;
  };

  QuadLight(std::array<gl::vec3, 4> vertices, const gl::vec3 &color,
            float intensity) {
    this->type = LightType::QUAD_LIGHT;
    this->vertices = vertices;
    this->color = color;
    this->intensity = intensity;
  };

  // uniformly sample a point on the light
  virtual gl::vec3 uniform_sample() const override {
    using namespace gl;
    float u = rand_num();
    float v = rand_num();
    vec3 v1 = vertices[1] - vertices[0];
    vec3 v2 = vertices[3] - vertices[0];
    vec3 p = vertices[0] + u * v1 + v * v2;
    return p;
  };

  virtual gl::vec3 get_sample(float u, float v) const override {
    using namespace gl;
    vec3 v1 = vertices[1] - vertices[0];
    vec3 v2 = vertices[3] - vertices[0];
    vec3 p = vertices[0] + u * v1 + v * v2;
    return p;
  };

  virtual gl::vec3 get_normal_at(const gl::vec3& p) const override {
    using namespace gl;
    vec3 v1 = vertices[1] - vertices[0];
    vec3 v2 = vertices[3] - vertices[0];
    vec3 normal = cross(v1, v2).normalize();
    return normal;
  }

  virtual float get_area() const override {
    using namespace gl;
    vec3 v1 = vertices[1] - vertices[0];
    vec3 v2 = vertices[3] - vertices[0];
    return cross(v1, v2).length();
  }

  ~QuadLight() = default;
};

class SphereLight : public Light {
public:
  gl::vec3 center;
  float radius;
  SphereLight(const gl::vec3 &center, float radius, const gl::vec3 &color,
              float intensity) {
    this->type = LightType::SPHERE_LIGHT;
    this->center = center;
    this->radius = radius;
    this->color = color;
    this->intensity = intensity;
  };

  SphereLight(std::shared_ptr<Sphere> sphere, const gl::vec3 &color,
              float intensity) {
    this->type = LightType::SPHERE_LIGHT;
    this->center = sphere->center;
    this->radius = sphere->radius;
    this->color = color;
    this->intensity = intensity;
  };

  virtual gl::vec3 uniform_sample() const override {
    using namespace gl;
    float u = rand_num();
    float v = rand_num();
    float theta = 2 * M_PI * u;
    float phi = std::acos(2 * v - 1);
    float x = std::sin(phi) * std::cos(theta);
    float y = std::sin(phi) * std::sin(theta);
    float z = std::cos(phi);
    vec3 p = center + radius * vec3(x, y, z);
    return p;
  };

  virtual gl::vec3 get_sample(float u, float v) const override {
    using namespace gl;
    float theta = 2 * M_PI * u;
    float phi = std::acos(2 * v - 1);
    float x = std::sin(phi) * std::cos(theta);
    float y = std::sin(phi) * std::sin(theta);
    float z = std::cos(phi);
    vec3 p = center + radius * vec3(x, y, z);
    return p;
  };

  virtual gl::vec3 get_normal_at(const gl::vec3& p) const override{
    using namespace gl;
    vec3 normal = (p - center).normalize();
    return normal;
  }

  virtual float get_area() const override {
    using namespace gl;
    return 2 * M_PI * radius * radius;
  }

  ~SphereLight() = default;
};

// using Lights =
//     std::tuple<std::vector<PointLight>, std::vector<DirectionalLight>>;
// Lights _get_lights_from_io(const LightIO *io);