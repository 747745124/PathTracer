#pragma once
#include "../utils/scene_io.hpp"
#include "./material.hpp"
#include "./object3D.hpp"
#include "./ray.hpp"
#include "./vertex.hpp"
#include "utils/aabb.hpp"
#include <memory>

using Materials = std::vector<std::shared_ptr<CustomMaterial>>;

struct HitRecord {
public:
  float t;
  gl::vec3 normal;
  gl::vec3 position;
  std::shared_ptr<CustomMaterial> material;
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

class Hittable : public Object3D {
public:
  virtual std::shared_ptr<HitRecord> intersect(const Ray &ray, float tmin,
                                               float tmax) const = 0;
  // prepare for motion blur (not required tho)
  // all objects static for now
  virtual AABB getAABB(float t0, float t1) = 0;
  ObjType objtype;
};

class Sphere : public Hittable {
public:
  float radius;
  gl::vec3 center;
  std::shared_ptr<CustomMaterial> material;
  Sphere(const gl::vec3 &center, float radius)
      : center(center), radius(radius) {
    this->objtype = ObjType::SPHERE_OBJ;
  };

  Sphere(const gl::vec3 &center, float radius,
         std::shared_ptr<CustomMaterial> material)
      : center(center), radius(radius), material(material) {
    this->objtype = ObjType::SPHERE_OBJ;
  };

  AABB getAABB(float t0, float t1) override {
    return AABB(this->center - gl::vec3(this->radius),
                this->center + gl::vec3(this->radius));
  }

  std::shared_ptr<HitRecord> intersect(const Ray &ray, float tmin = 0.0,
                                       float tmax = 10000.f) const override {
    auto ray_dir = ray.getDirection().normalize();
    auto ray_origin = ray.getOrigin();
    auto oc = ray_origin - this->center;
    auto a = dot(ray_dir, ray_dir);
    auto b = 2 * dot(ray_dir, oc);
    auto c = dot(oc, oc) - this->radius * this->radius;
    auto discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
      return nullptr;
    } else {
      auto delta = sqrtf(discriminant);
      auto t = (-b - delta) / (2 * a);
      if (t < tmin || t > tmax) {
        t = (-b + delta) / (2 * a);
        if (t < tmin || t > tmax) {
          return nullptr;
        }
      }

      auto hit_record = std::make_shared<HitRecord>();
      hit_record->t = t;
      hit_record->position = ray_origin + t * ray_dir;
      hit_record->set_normal(ray,
                             (hit_record->position - this->center).normalize());
      // calculate the uv coords of a sphere
      auto p = (hit_record->position - this->center).normalize();
      auto phi = atan2(p.z(), p.x());
      auto theta = asin(p.y());
      hit_record->texCoords =
          gl::vec2(1 - (phi + M_PI) / (2 * M_PI), (theta + M_PI / 2) / M_PI);
      // remap the uv coords, so that (0,0,1) is (0,0.5)
      hit_record->texCoords.u() =
          fmodf(hit_record->texCoords.u() + 0.75f, 1.0f);
      hit_record->material = this->material->getMaterial(hit_record->texCoords);
      return hit_record;
    }
  };
};

class Triangle : public Hittable {

public:
  AABB getAABB(float t0, float t1) override {
    auto min_x = std::min(
        {this->v0.position.x(), this->v1.position.x(), this->v2.position.x()});
    auto min_y = std::min(
        {this->v0.position.y(), this->v1.position.y(), this->v2.position.y()});
    auto min_z = std::min(
        {this->v0.position.z(), this->v1.position.z(), this->v2.position.z()});
    auto max_x = std::max(
        {this->v0.position.x(), this->v1.position.x(), this->v2.position.x()});
    auto max_y = std::max(
        {this->v0.position.y(), this->v1.position.y(), this->v2.position.y()});
    auto max_z = std::max(
        {this->v0.position.z(), this->v1.position.z(), this->v2.position.z()});
    return AABB(gl::vec3(min_x-1e3, min_y-1e3, min_z-1e3), gl::vec3(max_x, max_y, max_z));
  }

  Triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2)
      : v0(v0), v1(v1), v2(v2) {
    this->objtype = ObjType::POLYSET_OBJ;
  };

  Vertex v0, v1, v2;

  std::shared_ptr<HitRecord> intersect(const Ray &ray, float tmin = 0.0,
                                       float tmax = 10000.f) const override {
    const float epsilon = 1e-6;
    // using Möller–Trumbore intersection algorithm
    auto ray_dir = ray.getDirection().normalize();
    auto ray_origin = ray.getOrigin();
    auto edge1 = this->v1.position - this->v0.position;
    auto edge2 = this->v2.position - this->v0.position;
    auto h = cross(ray_dir, edge2);
    auto a = dot(edge1, h);

    // if isparallel
    if (a > -epsilon && a < epsilon)
      return nullptr;

    auto f = 1 / a;
    auto s = ray_origin - this->v0.position;
    auto u = f * dot(s, h);

    // out of the triangle
    if (u < 0.0 || u > 1.0)
      return nullptr;

    auto q = cross(s, edge1);
    auto v = f * dot(ray_dir, q);

    // out of the triangle
    if (v < 0.0 || u + v > 1.0)
      return nullptr;

    auto t = f * dot(edge2, q);
    if (t > tmin && t < tmax) {
      auto hit_record = std::make_shared<HitRecord>();
      hit_record->t = t;
      hit_record->position = ray_origin + t * ray_dir;

      auto hit_point = barycentric_lerp(v0, v1, v2, gl::vec2(u, v));
      hit_record->texCoords = hit_point.texCoords;
      hit_record->set_normal(ray, hit_point.normal);
      hit_record->material =
          hit_point.material->getMaterial(hit_record->texCoords);
      return hit_record;
    } else
      return nullptr;
  };
};

class PolySet {
public:
  std::vector<Triangle> triangles;

  PolySet(const std::vector<Triangle> &triangles) : triangles(triangles) {
    this->num_polys = triangles.size();
  };

  uint getNumPolys() const { return this->num_polys; }

  PolySetType getType() const { return this->polytype; }

private:
  uint num_polys;
  PolySetType polytype = PolySetType::POLYSET_TRI_MESH;
};

using Primitives = std::tuple<std::vector<Sphere>, std::vector<PolySet>>;
Primitives _get_primitives_from_io(const ObjIO *io);