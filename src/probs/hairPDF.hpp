#pragma once
#include "../base/materialMath.hpp"
#include "../probs/pdf.hpp"
#include "../utils/orthoBasis.hpp"

class HairMarschner;

class HairPDF : public PDF {
public:
  HairPDF(const HairMarschner &hair_mat, const OrthoBasis &basis,
          const gl::vec3 &wo_world);
  ~HairPDF() = default;

  gl::vec3 get() const override;

  float at(const gl::vec3 &wi_world) const override;

private:
  const HairMarschner &hair_mat;
  const OrthoBasis basis;
  const gl::vec3 wo_local;
};