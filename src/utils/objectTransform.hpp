#pragma once
#include "../base/compound.hpp"
#include "../base/objectList.hpp"
#include "../base/primitive.hpp"

class Translate : public Hittable {
public:
  Translate(std::shared_ptr<Hittable> object, const gl::vec3 &offset)
      : object(object), offset(offset) {
    this->objtype = object->objtype;
  };

  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
                 float tmax = 10000.f) const override {

    Ray new_ray(ray.getOrigin() - offset, ray.getDirection());
    if (!object->intersect(new_ray, hit_record, tmin, tmax)) {
      return false;
    }

    hit_record.position += offset;
    return true;
  };

  AABB getAABB(float t0, float t1) override {
    AABB box = object->getAABB(t0, t1);
    return AABB(box.get_min() + offset, box.get_max() + offset);
  };

  std::shared_ptr<Hittable> object;
  gl::vec3 offset;
};

template <Axis axis> class Rotate : public Hittable {
public:
  Rotate(std::shared_ptr<Hittable> object, float angle)
      : object(object), angle(angle) {
    this->objtype = object->objtype;
    sin_theta = std::sin(angle);
    cos_theta = std::cos(angle);
  };

  AABB getAABB(float t0, float t1) override;
  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
                 float tmax = 10000.f) const override;

  std::shared_ptr<Hittable> object;
  float angle;
  float sin_theta;
  float cos_theta;
};