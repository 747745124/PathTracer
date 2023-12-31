#pragma once
#include "../probs/pdf.hpp"
#include "../probs/random.hpp"
#include "../utils/matrix.hpp"
#include "../utils/orthoBasis.hpp"
#include "../utils/scene_io.hpp"
#include "./ray.hpp"
#include "./texture.hpp"

class Material;
class Lambertian;
class Mirror;
class Dielectric;

namespace gl {
static std::shared_ptr<ConstantTexture> DefaultTexture =
    std::make_shared<ConstantTexture>(gl::vec3(1.0f));
static std::shared_ptr<Lambertian> DefaultMaterial =
    std::make_shared<Lambertian>(DefaultTexture);
}; // namespace gl

// determine whether the ray is specular
struct ScatterRecord {
  Ray specular_ray;
  bool is_specular;
  gl::vec3 attenuation;
  std::shared_ptr<PDF> pdf_ptr;
};

struct HitRecord {
public:
  float t;
  gl::vec3 normal;
  gl::vec3 position;
  std::shared_ptr<Material> material;
  gl::vec2 texCoords = gl::vec2(0.0f);
  // Ref: rt in one weeknd
  // This is used to determine whether the ray is inside or outside the object
  // As we want have the normal always point against the ray
  bool is_inside;
  void set_normal(const Ray &ray, const gl::vec3 &n) {
    this->is_inside = dot(ray.getDirection(), n) < 0;
    this->normal = this->is_inside ? n : -n;
  }
};

class Material {
public:
  virtual bool scatter(const Ray &ray_in, HitRecord &rec,
                       ScatterRecord &srec) const {
    return false;
  }

  virtual float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                            const Ray &scattered) const {
    return 0.0f;
  }

  virtual gl::vec3 emit(const Ray &ray_in, HitRecord &rec) {
    return gl::vec3(0.0f);
  }
};

class Lambertian : public Material {

public:
  Lambertian(std::shared_ptr<Texture2D> a) : albedo(a){};
  Lambertian(const gl::vec3 &a)
      : albedo(std::make_shared<ConstantTexture>(a)){};
  // scatter the ray with lambertian reflection
  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {
    srec.is_specular = false;
    srec.attenuation = albedo->getTexelColor(rec.texCoords);
    srec.pdf_ptr = std::make_shared<CosinePDF>(rec.normal);
    return true;
  }

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered) const override {
    float cosine = dot(rec.normal, scattered.getDirection().normalize());
    return std::max(cosine / M_PI, 0.0);
  }

  std::shared_ptr<Texture2D> albedo;
};

class Mirror : public Material {
public:
  Mirror(const gl::vec3 &a, float f = 0.f) : albedo(a) { fuzz = f < 1 ? f : 1; }
  // it's hard to use a delta functinon as BRDF,
  // since the floating point precision causes trouble
  // This design is a reference from Raytracing the rest of your life
  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {
    gl::vec3 reflected =
        reflect(ray_in.getDirection().normalize(), rec.normal) +
        gl::on_sphere_random_vec(fuzz);
    srec.specular_ray = Ray(rec.position, reflected);
    srec.attenuation = albedo;
    srec.is_specular = true;
    srec.pdf_ptr = nullptr;
    return true;
  }

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered) const override {
    return 0.0f;
  };

  gl::vec3 albedo;
  float fuzz;
};

class Phong : public Material {
public:
  Phong(const gl::vec3 &diffuse, const gl::vec3 &specular,
        const gl::vec3 &ambient, float shininess)
      : diffuse(diffuse), specular(specular), ambient(ambient),
        shininess(shininess){};

  gl::vec3 diffuse;
  gl::vec3 specular;
  gl::vec3 ambient;
  float shininess;

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {
    float p = gl::C_rand();

    if (p < shininess) {
      srec.is_specular = true;
      srec.attenuation = specular + ambient;
      srec.pdf_ptr = nullptr;
      srec.specular_ray = Ray(
          rec.position, reflect(ray_in.getDirection().normalize(), rec.normal));
      return true;
    }

    srec.is_specular = false;
    srec.attenuation = diffuse + ambient;
    srec.pdf_ptr = std::make_shared<CosinePDF>(rec.normal);
    return true;
  }

  float scatter_pdf(const Ray &ray_in, const HitRecord &rec,
                    const Ray &scattered) const override {
    float p = gl::C_rand();
    if (p < shininess) {
      return 0.0f;
    }
    float cosine = dot(rec.normal, scattered.getDirection().normalize());
    return std::max(cosine / M_PI, 0.0);
  }
};

class Dielectric : public Material {
public:
  float ior;
  Dielectric(float ior) : ior(ior){};

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {
    srec.is_specular = true;
    srec.pdf_ptr = nullptr;
    srec.attenuation = gl::vec3(1.0f);
    float ri_ro = rec.is_inside ? 1.0f / ior : ior;
    bool is_refract = false;
    gl::vec3 out_ray = refract(ray_in.getDirection().normalize(), rec.normal,
                               ri_ro, is_refract);
    srec.specular_ray = Ray(rec.position, out_ray);
    return true;
  };
};

class DiffuseEmitter : public Material {
public:
  DiffuseEmitter(std::shared_ptr<Texture2D> a, float intensity = 1.0f)
      : _text(a), _intensity(intensity){};
  DiffuseEmitter(const gl::vec3 &a, float intensity = 1.0f)
      : _text(std::make_shared<ConstantTexture>(a)), _intensity(intensity){};

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {
    return false;
  }

  gl::vec3 emit(const Ray &ray_in, HitRecord &rec) override {

    // use unidirectional light or not
    //  if (rec.is_inside)
    return _text->getTexelColor(rec.texCoords) * _intensity;
    // return gl::vec3(0.0f);
  }

private:
  std::shared_ptr<Texture2D> _text;
  float _intensity;
};

class Isotropic : public Material {
public:
  Isotropic(std::shared_ptr<Texture2D> a) : _text(a){};
  Isotropic(const gl::vec3 &a) : _text(std::make_shared<ConstantTexture>(a)){};

  bool scatter(const Ray &ray_in, HitRecord &rec,
               ScatterRecord &srec) const override {
    // scatter the ray with lambertian reflection
    // any direction scatter with equal probability
    // ray_scattered = Ray(rec.position, gl::sphere_random_vec());
    // attenuation = _text->getTexelColor(rec.texCoords.u(), rec.texCoords.v());
    // return true;
    return true;
  }

private:
  std::shared_ptr<Texture2D> _text;
};

//----------------------------------------------------------------
// Legacy material for Whitted RT
// will be refactored with the current material class
//----------------------------------------------------------------
//
// struct CustomMaterial {
//   gl::vec3 diff_color = gl::vec3(0.0f, 0.0f, 0.0f);
//   gl::vec3 ambient_color = gl::vec3(0.0f, 0.0f, 0.0f);
//   gl::vec3 spec_color = gl::vec3(0.0f, 0.0f, 0.0f);
//   gl::vec3 emissive_color = gl::vec3(0.0f, 0.0f, 0.0f);

//   float shininess = 0.2f;
//   float ktran = 0.f;

//   CustomMaterial() = default;

//   CustomMaterial(gl::vec3 diff_color, gl::vec3 ambient_color,
//                  gl::vec3 spec_color, gl::vec3 emissive_color, float
//                  shininess, float ktran) {
//     this->diff_color = diff_color;
//     this->ambient_color = ambient_color;
//     this->spec_color = spec_color;
//     this->emissive_color = emissive_color;
//     this->shininess = shininess;
//     this->ktran = ktran;
//   }

//   CustomMaterial(const MaterialIO *io) {
//     this->ambient_color = io->ambColor;
//     this->diff_color = io->diffColor;
//     this->spec_color = io->specColor;
//     this->emissive_color = io->emissColor;
//     this->shininess = io->shininess;
//     this->ktran = io->ktran;
//   }

//   CustomMaterial(const MaterialIO &io) {
//     this->ambient_color = io.ambColor;
//     this->diff_color = io.diffColor;
//     this->spec_color = io.specColor;
//     this->emissive_color = io.emissColor;
//     this->shininess = io.shininess;
//     this->ktran = io.ktran;
//   }

//   virtual std::shared_ptr<CustomMaterial> getMaterial(float u, float v) {
//     auto material = std::make_shared<CustomMaterial>(*this);
//     return material;
//   }

//   virtual std::shared_ptr<CustomMaterial> getMaterial(gl::vec2 uv) {
//     auto material = std::make_shared<CustomMaterial>(*this);
//     return material;
//   }

//   ~CustomMaterial() = default;
// };

// struct CheckerReflector : public CustomMaterial {
//   CheckerReflector(gl::vec3 color1, gl::vec3 color2, float scale) {
//     this->_texture = std::make_shared<CheckerTexture>(color1, color2, scale);
//   }

//   CheckerReflector(float scale = 20.f) {
//     this->_texture = std::make_shared<CheckerTexture>(scale);
//   }

//   // checker reflector
//   std::shared_ptr<CustomMaterial> getMaterial(float u, float v) override {
//     auto material = std::make_shared<CustomMaterial>();
//     if (this->_texture->getTexelColor(u, v) == gl::vec3(0.0f, 0.0f, 0.0f)) {
//       material->diff_color = gl::vec3(0.1f, 0.1f, 0.1f);
//       material->spec_color = gl::vec3(0.5f, 0.4f, 0.5f);
//     } else {
//       material->diff_color = gl::vec3(1.f, 1.f, 1.f);
//     }
//     return material;
//   }

//   std::shared_ptr<CustomMaterial> getMaterial(gl::vec2 uv) override {
//     return getMaterial(uv.u(), uv.v());
//   }

// private:
//   // only one image texture here
//   std::shared_ptr<CheckerTexture> _texture;
// };

// struct LambertianMaterial : public CustomMaterial {
//   LambertianMaterial() = default;

//   LambertianMaterial(const std::string &filepath) {
//     this->_texture = std::make_shared<ImageTexture>(filepath);
//   }

//   std::shared_ptr<CustomMaterial> getMaterial(float u, float v) override {
//     auto material = std::make_shared<CustomMaterial>();
//     material->diff_color = this->_texture->getTexelColor(u, v);
//     return material;
//   }

//   std::shared_ptr<CustomMaterial> getMaterial(gl::vec2 uv) override {
//     return getMaterial(uv.u(), uv.v());
//   }

// private:
//   // only one image texture here
//   std::shared_ptr<ImageTexture> _texture;
// };

// struct PhongMaterial : public CustomMaterial {
//   PhongMaterial() = default;
//   PhongMaterial(std::vector<std::string> &filepaths) {
//     this->_albedo = std::make_shared<ImageTexture>(filepaths[0]);
//     this->_specular = std::make_shared<ImageTexture>(filepaths[1]);
//     this->_ambient = std::make_shared<ImageTexture>(filepaths[2]);
//   }

//   std::shared_ptr<CustomMaterial> getMaterial(float u, float v) override {
//     auto material = std::make_shared<CustomMaterial>();
//     material->diff_color = this->_albedo->getTexelColor(u, v);
//     material->spec_color = this->_specular->getTexelColor(u, v);
//     material->ambient_color = this->_ambient->getTexelColor(u, v).x();
//     material->shininess = 0.2f;
//     material->ktran = this->ktran;
//     material->emissive_color = this->emissive_color;
//     return material;
//   }

//   std::shared_ptr<CustomMaterial> getMaterial(gl::vec2 uv) override {
//     return getMaterial(uv.u(), uv.v());
//   }

// private:
//   std::shared_ptr<ImageTexture> _albedo;
//   std::shared_ptr<ImageTexture> _specular;
//   std::shared_ptr<ImageTexture> _ambient;
// };

// struct PBRMaterial {
//   gl::vec3 albedo = gl::vec3(0.5f);
//   float metallic = 0.3f;
//   float roughness = 0.3f;
//   float ao = 0.4f;

//   PBRMaterial() = default;
//   float GGX(float NdotH) const {
//     NdotH = std::max(NdotH, 0.f);
//     float a = roughness * roughness;
//     float a2 = a * a;
//     float NdotH2 = NdotH * NdotH;
//     float nom = a2;
//     float denom = (NdotH2 * (a2 - 1.f) + 1.f);
//     denom = M_PI * denom * denom;
//     return nom / denom;
//   };

//   gl::vec3 FresnelSchlick(float cos_theta, gl::vec3 F0) const {
//     return F0 + (gl::vec3(1.0) - F0) * pow(1.0 - cos_theta, 5.0);
//   };

//   float GeometrySchlickGGX(float NdotV) const {
//     float a = roughness;
//     float k = (a * a) / 2.0;

//     float nom = NdotV;
//     float denom = NdotV * (1.0 - k) + k;

//     return nom / denom;
//   };

//   float GeometrySmith(gl::vec3 N, gl::vec3 V, gl::vec3 L) const {
//     float NdotV = std::max(dot(N, V), 0.0f);
//     float NdotL = std::max(dot(N, L), 0.0f);
//     float ggx2 = GeometrySchlickGGX(NdotV);
//     float ggx1 = GeometrySchlickGGX(NdotL);

//     return ggx1 * ggx2;
//   };

//   float D_Ashikhmin(float NoH) {
//     // Ashikhmin 2007, "Distribution-based BRDFs"
//     // Cloth BRDF
//     float a2 = roughness * roughness;
//     float cos2h = NoH * NoH;
//     float sin2h = std::max(1.0 - cos2h, 0.0078125);
//     float sin4h = sin2h * sin2h;
//     float cot2 = -cos2h / (a2 * sin2h);
//     return 1.0 / (M_PI * (4.0 * a2 + 1.0) * sin4h) * (4.0 * exp(cot2) +
//     sin4h);
//   }

//   // ue4 2013
//   float shadowMaskingUnreal(gl::vec3 N, gl::vec3 V, gl::vec3 L) {
//     float NdotV = std::max(dot(N, V), 0.0f);
//     float NdotL = std::max(dot(N, L), 0.0f);
//     float ggx1 = GeometrySchlickGGX(NdotL);
//     float ggx2 = GeometrySchlickGGX(NdotV);
//     return ggx1 * ggx2;
//   }

//   gl::vec3 ImportanceSamplingGGX(gl::vec2 Xi, gl::vec3 N) {
//     using namespace gl;
//     float a = roughness * roughness;
//     float phi = 2.0f * M_PI * Xi.x();
//     float cos_theta = sqrt((1.0f - Xi.y()) / (1.0f + (a * a - 1.0f) *
//     Xi.y())); float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

//     // spherical coordinates to cartesian coordinates
//     vec3 H;
//     H.x() = cos(phi) * sin_theta;
//     H.y() = sin(phi) * sin_theta;
//     H.z() = cos_theta;

//     // tangent space to world space
//     vec3 up =
//         fabs(N.z()) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f,
//         0.0f);
//     vec3 tangent = normalize(cross(up, N));
//     vec3 bitangent = cross(N, tangent);

//     vec3 sample_vec = tangent * H.x() + bitangent * H.y() + N * H.z();
//     return normalize(sample_vec);
//   };
// };
