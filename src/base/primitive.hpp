#pragma once
#include "../utils/scene_io.hpp"
#include "./material.hpp"
#include "./object3D.hpp"
#include "./ray.hpp"
#include "./vertex.hpp"
#include "utils/aabb.hpp"
#include <memory>

using Materials = std::vector<std::shared_ptr<Material>>;
enum class IntersectionMode { DEFAULT, CUSTOM };
extern int hit_count;

class Hittable : public Object3D {
public:
  virtual bool intersect(const Ray &ray, HitRecord &record, float tmin,
                         float tmax) const = 0;
  // prepare for motion blur (not required tho)
  // all objects static for now
  virtual AABB getAABB(float t0, float t1) = 0;
  virtual void setIntersectionMode(IntersectionMode mode) {
    this->intersection_mode = mode;
  };
  ObjType objtype;
  IntersectionMode intersection_mode = IntersectionMode::DEFAULT;
};

class Sphere : public Hittable {
public:
  float radius;
  gl::vec3 center;
  std::shared_ptr<Material> material;
  Sphere(const gl::vec3 &center, float radius)
      : center(center), radius(radius) {
    this->objtype = ObjType::SPHERE_OBJ;
  };

  Sphere(const gl::vec3 &center, float radius,
         std::shared_ptr<Material> material)
      : center(center), radius(radius), material(material) {
    this->objtype = ObjType::SPHERE_OBJ;
  };

  AABB getAABB(float t0, float t1) override {
    AABB aabb(this->center - gl::vec3(this->radius),
              this->center + gl::vec3(this->radius));
    return aabb;
  }

  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0,
                 float tmax = 10000.f) const override {
    hit_count++;
    auto ray_dir = ray.getDirection().normalize();
    auto ray_origin = ray.getOrigin();
    auto oc = ray_origin - this->center;
    auto a = dot(ray_dir, ray_dir);
    auto b = 2 * dot(ray_dir, oc);
    auto c = dot(oc, oc) - this->radius * this->radius;
    auto discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
      return false;
    auto delta = sqrtf(discriminant);
    auto t = (-b - delta) / (2 * a);
    if (t < tmin || t > tmax) {
      t = (-b + delta) / (2 * a);
      if (t < tmin || t > tmax) {
        return false;
      }
    }

    switch (this->intersection_mode) {
    case IntersectionMode::DEFAULT: {
      hit_record.t = t;
      hit_record.position = ray_origin + t * ray_dir;
      hit_record.set_normal(ray,
                            (hit_record.position - this->center).normalize());
      // calculate the uv coords of a sphere
      auto p = (hit_record.position - this->center).normalize();
      auto phi = atan2(p.z(), p.x());
      auto theta = asin(p.y());
      hit_record.texCoords =
          gl::vec2(1 - (phi + M_PI) / (2 * M_PI), (theta + M_PI / 2) / M_PI);
      // remap the uv coords, so that (0,0,1) is (0,0.5)
      hit_record.texCoords.u() = fmodf(hit_record.texCoords.u() + 0.75f, 1.0f);
      hit_record.material =
          this->material == nullptr ? gl::DefaultMaterial : this->material;
      return true;
    }

    // an checkerboard pattern
    case IntersectionMode::CUSTOM: {
      hit_record.t = t;
      hit_record.position = ray_origin + t * ray_dir;
      hit_record.set_normal(ray,
                            (hit_record.position - this->center).normalize());
      // calculate the uv coords of a sphere
      auto p = (hit_record.position - this->center).normalize();
      auto phi = atan2(p.z(), p.x());
      auto theta = asin(p.y());
      hit_record.texCoords =
          gl::vec2(1 - (phi + M_PI) / (2 * M_PI), (theta + M_PI / 2) / M_PI);
      // remap the uv coords, so that (0,0,1) is (0,0.5)
      hit_record.texCoords.u() = fmodf(hit_record.texCoords.u() + 0.75f, 1.0f);
      hit_record.material =
          this->material == nullptr ? gl::DefaultMaterial : this->material;

      if ((int)(hit_record.texCoords.u() * 10) % 2 ==
          (int)(hit_record.texCoords.v() * 10) % 2)
        return false;

      return true;
    }
    default: {
      throw std::runtime_error("Invalid intersection mode");
    }
    }
  };
};

class AARectangle : public Hittable {
public:
  float _d0_min, _d0_max, _d1_min, _d1_max, _k;
  std::shared_ptr<Material> material;

  AARectangle(float k, float d0_min, float d0_max, float d1_min, float d1_max)
      : _d0_min(d0_min), _d0_max(d0_max), _d1_min(d1_min), _d1_max(d1_max),
        _k(k) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  AARectangle(float k, float d0_min, float d0_max, float d1_min, float d1_max,
              std::shared_ptr<Material> material)
      : _d0_min(d0_min), _d0_max(d0_max), _d1_min(d1_min), _d1_max(d1_max),
        _k(k), material(material) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  virtual AABB getAABB(float t0, float t1) = 0;
  virtual bool intersect(const Ray &ray, HitRecord &hit_record,
                         float tmin = 0.0001, float tmax = 10000.f) const = 0;
};

// an axis aligned rectangle, based on xy plane and
// z value
class XYRectangle : public AARectangle {
public:
  XYRectangle(float k, float x0, float x1, float y0, float y1)
      : AARectangle(k, x0, x1, y0, y1) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  XYRectangle(float k, float x0, float x1, float y0, float y1,
              std::shared_ptr<Material> material)
      : AARectangle(k, x0, x1, y0, y1, material) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  AABB getAABB(float t0, float t1) override {
    AABB aabb(gl::vec3(this->_d0_min, this->_d1_min, this->_k - 0.001),
              gl::vec3(this->_d0_max, this->_d1_max, this->_k + 0.001));
    return aabb;
  };

  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
                 float tmax = 10000.f) const override {
    auto ray_dir = ray.getDirection().normalize();
    auto ray_origin = ray.getOrigin();
    auto t = (_k - ray_origin.z()) / ray_dir.z();
    if (t < tmin || t > tmax)
      return false;

    auto x = ray_origin.x() + t * ray_dir.x();
    auto y = ray_origin.y() + t * ray_dir.y();
    if (x < _d0_min || x > _d0_max || y < _d1_min || y > _d1_max)
      return false;

    switch (this->intersection_mode) {
    case IntersectionMode::DEFAULT: {
      hit_record.t = t;
      hit_record.position = ray_origin + t * ray_dir;
      hit_record.set_normal(ray, gl::vec3(0.0f, 0.0f, 1.0f));
      hit_record.texCoords = gl::vec2((x - _d0_min) / (_d0_max - _d0_min),
                                      (y - _d1_min) / (_d1_max - _d1_min));
      hit_record.material =
          this->material == nullptr ? gl::DefaultMaterial : this->material;
      return true;
    } break;
    case IntersectionMode::CUSTOM: {
      std::cout << "Custom intersection mode not implemented yet" << std::endl;
      return false;
    } break;
    default: {
      throw std::runtime_error("Invalid intersection mode");
    }
    };
  };
};

// an axis aligned rectangle, based on yz plane and
// x value
class YZRectangle : public AARectangle {
public:
  YZRectangle(float k, float y0, float y1, float z0, float z1)
      : AARectangle(k, y0, y1, z0, z1) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  YZRectangle(float k, float y0, float y1, float z0, float z1,
              std::shared_ptr<Material> material)
      : AARectangle(k, y0, y1, z0, z1, material) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  AABB getAABB(float t0, float t1) override {
    AABB aabb(gl::vec3(this->_k - 0.001, this->_d0_min, this->_d1_min),
              gl::vec3(this->_k + 0.001, this->_d0_max, this->_d1_max));
    return aabb;
  };

  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
                 float tmax = 10000.f) const override {
    auto ray_dir = ray.getDirection().normalize();
    auto ray_origin = ray.getOrigin();
    auto t = (_k - ray_origin.x()) / ray_dir.x();
    if (t < tmin || t > tmax)
      return false;

    auto y = ray_origin.y() + t * ray_dir.y();
    auto z = ray_origin.z() + t * ray_dir.z();
    if (y < _d0_min || y > _d0_max || z < _d1_min || z > _d1_max)
      return false;

    switch (this->intersection_mode) {
    case IntersectionMode::DEFAULT: {
      hit_record.t = t;
      hit_record.position = ray_origin + t * ray_dir;
      hit_record.set_normal(ray, gl::vec3(1.0f, 0.0f, 0.0f));
      hit_record.texCoords = gl::vec2((y - _d0_min) / (_d0_max - _d0_min),
                                      (z - _d1_min) / (_d1_max - _d1_min));
      hit_record.material =
          this->material == nullptr ? gl::DefaultMaterial : this->material;
      return true;
    } break;
    case IntersectionMode::CUSTOM: {
      std::cout << "Custom intersection mode not implemented yet" << std::endl;
      return false;
    } break;
    default: {
      throw std::runtime_error("Invalid intersection mode");
    }
    };
  };
};

class XZRectangle : public AARectangle {
public:
  XZRectangle(float k, float x0, float x1, float z0, float z1)
      : AARectangle(k, x0, x1, z0, z1) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  XZRectangle(float k, float x0, float x1, float z0, float z1,
              std::shared_ptr<Material> material)
      : AARectangle(k, x0, x1, z0, z1, material) {
    this->objtype = ObjType::RECTANGLE_OBJ;
  };

  AABB getAABB(float t0, float t1) override {
    AABB aabb(gl::vec3(this->_d0_min, this->_k - 0.001, this->_d1_min),
              gl::vec3(this->_d0_max, this->_k + 0.001, this->_d1_max));
    return aabb;
  };

  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
                 float tmax = 10000.f) const override {
    auto ray_dir = ray.getDirection().normalize();
    auto ray_origin = ray.getOrigin();
    auto t = (_k - ray_origin.y()) / ray_dir.y();
    if (t < tmin || t > tmax)
      return false;

    auto x = ray_origin.x() + t * ray_dir.x();
    auto z = ray_origin.z() + t * ray_dir.z();
    if (x < _d0_min || x > _d0_max || z < _d1_min || z > _d1_max)
      return false;

    switch (this->intersection_mode) {
    case IntersectionMode::DEFAULT: {
      hit_record.t = t;
      hit_record.position = ray_origin + t * ray_dir;
      hit_record.set_normal(ray, gl::vec3(0.0f, 1.0f, 0.0f));
      hit_record.texCoords = gl::vec2((x - _d0_min) / (_d0_max - _d0_min),
                                      (z - _d1_min) / (_d1_max - _d1_min));
      hit_record.material =
          this->material == nullptr ? gl::DefaultMaterial : this->material;
      return true;
    } break;
    case IntersectionMode::CUSTOM: {
      std::cout << "Custom intersection mode not implemented yet" << std::endl;
      return false;
    } break;
    default: {
      throw std::runtime_error("Invalid intersection mode");
    }
    };
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
    AABB aabb(gl::vec3(min_x, min_y, min_z - 0.01),
              gl::vec3(max_x, max_y, max_z));
    return aabb;
  }

  Triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2)
      : v0(v0), v1(v1), v2(v2) {
    this->objtype = ObjType::POLYSET_OBJ;
  };

  Vertex v0, v1, v2;

  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
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
      return false;

    auto f = 1 / a;
    auto s = ray_origin - this->v0.position;
    auto u = f * dot(s, h);

    // out of the triangle
    if (u < 0.0 || u > 1.0)
      return false;

    auto q = cross(s, edge1);
    auto v = f * dot(ray_dir, q);

    // out of the triangle
    if (v < 0.0 || u + v > 1.0)
      return false;

    auto t = f * dot(edge2, q);

    switch (this->intersection_mode) {
    case IntersectionMode::DEFAULT: {

      if (t > tmin && t < tmax) {
        hit_record.t = t;
        hit_record.position = ray_origin + t * ray_dir;
        hit_record.texCoords = gl::vec2(u, v);
        hit_record.set_normal(ray, cross(edge1, edge2).normalize());
        hit_record.material =
            v0.material == nullptr ? gl::DefaultMaterial : v0.material;
        // auto hit_point = barycentric_lerp(v0, v1, v2, gl::vec2(u, v));
        // hit_record->texCoords = hit_point.texCoords;
        // hit_record->set_normal(ray, hit_point.normal);
        // hit_record->material =
        //     hit_point.material == nullptr
        //         ? std::make_shared<Material>()
        //         :
        return true;
      } else
        return false;
    } break;
    case IntersectionMode::CUSTOM: {
      std::cout << "Custom intersection mode not implemented yet" << std::endl;
      return false;
    } break;
    default: {
      throw std::runtime_error("Invalid intersection mode");
    }
    };
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