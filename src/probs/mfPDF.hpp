#pragma once
#include "../base/materialMath.hpp"
#include "../probs/pdf.hpp"
#include "../utils/orthoBasis.hpp"

class MicrofacetPDF : public PDF {
public:
  MicrofacetPDF(const TrowbridgeReitzDistribution &d, const OrthoBasis &basis,
                const gl::vec3 &wi_world)
      // flip wi so that in local-space it points *out* (z>0)
      : distrib(d), onb(basis), wi_local(onb.toLocal(-wi_world.normalize())) {}

  // sample an outgoing direction in world space
  gl::vec3 get() const override {
    // draw u1,u2 in [0,1)
    float u1 = gl::rand_num(), u2 = gl::rand_num();
    // sample half‐vector in local
    gl::vec3 m_local = distrib.sample_wm(wi_local, gl::vec2(u1, u2));
    // here we assume the wi is pointing into the surface
    gl::vec3 wo_local = reflect(-wi_local, m_local);
    // if it went below the surface, just flip
    if (wo_local.z() <= 0)
      wo_local.z() = -wo_local.z();
    // back to world
    return onb.toWorld(wo_local);
  }

  // PDF for a given outgoing direction (world→local)
  float at(const gl::vec3 &wo_world) const override {
    gl::vec3 wo = onb.toLocal(wo_world.normalize());
    if (wo.z() <= 0)
      return 0;
    // half‐vector
    gl::vec3 m = (wi_local + wo).normalize();
    auto denominator = 4 * std::abs(dot(wi_local, m));
    if (denominator < 1e-6)
      return 0;
    // microfacet PDF = D(m) * |cosθ_m| / (4 * |dot(wi,m)|)
    return distrib.PDF(wi_local, m) * std::abs(m.z()) / denominator;
  }

private:
  TrowbridgeReitzDistribution distrib;
  OrthoBasis onb;
  gl::vec3 wi_local;
};
