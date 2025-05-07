#pragma once
#include "base/colors.hpp"
#include "material/material.hpp"
#include "utils/matrix.hpp"

class RaySplitDispersion : public Material {
public:
  // IORs for red, green, and blue wavelengths
  float ior_r;
  float ior_g;
  float ior_b;

  RaySplitDispersion(float ior_r, float ior_g, float ior_b)
      : ior_r(ior_r), ior_g(ior_g), ior_b(ior_b) {}

  bool
  scatter(const Ray &ray_in, HitRecord &rec, ScatterRecord &srec,
          float uc = gl::rand_num(),
          const gl::vec2 &u = {gl::rand_num(), gl::rand_num()},
          TransportMode mode = TransportMode::Radiance,
          BxDFReflTransFlags flags = BxDFReflTransFlags::All) const override {
    srec.pdf_ptr = nullptr;

    // Calculate refraction for each color channel
    bool is_refract_r = false, is_refract_g = false, is_refract_b = false;
    float ri_ro_r = rec.is_inside ? 1.0f / ior_r : ior_r;
    float ri_ro_g = rec.is_inside ? 1.0f / ior_g : ior_g;
    float ri_ro_b = rec.is_inside ? 1.0f / ior_b : ior_b;

    gl::vec3 out_ray_r = refract(ray_in.getDirection().normalize(), rec.normal,
                                 ri_ro_r, is_refract_r);
    gl::vec3 out_ray_g = refract(ray_in.getDirection().normalize(), rec.normal,
                                 ri_ro_g, is_refract_g);
    gl::vec3 out_ray_b = refract(ray_in.getDirection().normalize(), rec.normal,
                                 ri_ro_b, is_refract_b);

    // Check if any color channel can refract
    bool can_refract = is_refract_r || is_refract_g || is_refract_b;

    if (can_refract) {
      // Signal that we need ray-splitting
      srec.sampled_type = BxDFFlags::SpecularTransmission;
      // Store the refracted directions in the scatter record
      srec.split_rays = {Ray(rec.position, out_ray_r),
                         Ray(rec.position, out_ray_g),
                         Ray(rec.position, out_ray_b)};
      // Store the attenuation for each ray
      srec.split_attenuations = {
          gl::vec3(1.0f, 0.0f, 0.0f), // Red
          gl::vec3(0.0f, 1.0f, 0.0f), // Green
          gl::vec3(0.0f, 0.0f, 1.0f)  // Blue
      };
      // Store which rays are valid
      srec.split_valid = {is_refract_r, is_refract_g, is_refract_b};
    } else {
      // Total Internal Reflection - use reflection
      gl::vec3 reflected =
          reflect(ray_in.getDirection().normalize(), rec.normal);
      srec.sampled_ray = Ray(rec.position, reflected);
      srec.sampled_type = BxDFFlags::SpecularReflection;
      srec.attenuation = gl::vec3(1.0f); // Full reflection
    }

    srec.pdf_val = 0.0f; // Delta function
    return true;
  }
};