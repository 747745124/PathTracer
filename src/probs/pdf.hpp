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

class PhongLobePDF : public PDF {
  public:
      // Constructor takes the perfect reflection direction and the shininess exponent.
      PhongLobePDF(const gl::vec3 &R, float shininess)
          : R(normalize(R)), shininess(shininess), onb(R) {}
  
      // Return the PDF value for a given outgoing direction.
      float at(const gl::vec3 &direction) const override {
          float cosine = std::max(dot(normalize(direction), R), 0.0f);
          return (shininess + 1.0f) / (2.0f * M_PI) * pow(cosine, shininess);
      }
  
      // Sample a new direction according to the Phong-lobe distribution.
      gl::vec3 get() const override {
          float u1 = gl::rand_num();
          float u2 = gl::rand_num();
          // Invert the cumulative distribution for cosine-power:
          float theta = acos(pow(u1, 1.0f / (shininess + 1.0f)));
          float phi = 2.0f * M_PI * u2;
          // Convert spherical coordinates to a local direction.
          gl::vec3 localDir = gl::sphericalDirection(theta, phi);
          return normalize(onb.at(localDir));
      }
  
  private:
      gl::vec3 R;          
      float shininess;     
      OrthoBasis onb;     
  };
  