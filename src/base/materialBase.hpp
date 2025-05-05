#pragma once
#include "../probs/pdf.hpp"
#include "../probs/random.hpp"
#include "./ray.hpp"

class Material;
enum class TransportMode { Radiance, Importance };
enum BxDFFlags {
  Reflection = 1 << 0,
  Transmission = 1 << 1,
  Diffuse = 1 << 2,
  Glossy = 1 << 3,
  Specular = 1 << 4,
  DiffuseReflection = Diffuse | Reflection,
  DiffuseTransmission = Diffuse | Transmission,
  GlossyReflection = Glossy | Reflection,
  GlossyTransmission = Glossy | Transmission,
  SpecularReflection = Specular | Reflection,
  SpecularTransmission = Specular | Transmission,
  All = ~0u
};

// determine whether the ray is specular
struct ScatterRecord {
  Ray sampled_ray;
  uint32_t sampled_type = BxDFFlags::All;
  float pdf_val = 0.0f;
  gl::vec3 attenuation;
  std::shared_ptr<PDF> pdf_ptr;

  bool is_specular() { return this->sampled_type & BxDFFlags::Specular; }
};

struct HitRecord {
public:
  float t;
  gl::vec3 normal;
  gl::vec3 position;
  std::shared_ptr<Material> material;
  gl::vec2 texCoords = gl::vec2(0.0f);
  gl::vec3 hair_tangent = gl::vec3(0.0f);
  // the tangent of the hair, used for hair shading

  // Ref: rt in one weeknd
  // This is used to determine whether the ray is inside or outside the object
  // As we want have the normal always point against the ray
  bool is_inside;
  void set_normal(const Ray &ray, const gl::vec3 &n) {
    this->is_inside = dot(ray.getDirection(), n) < 0;
    this->normal = this->is_inside ? n : -n;
  }
};
