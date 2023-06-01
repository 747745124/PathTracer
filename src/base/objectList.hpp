#pragma once
#include "../utils/utility.hpp"
#include "./camera.hpp"
#include "./light.hpp"
#include "./primitive.hpp"
#include "../utils/aabb.hpp"
// a manager for hittable objects
class ObjectList : public Hittable {
public:
  ObjectList() = default;
  ~ObjectList() = default;
  ObjectList(const Primitives &prim) {
    auto &[spheres, polysets] = prim;
    for (const auto &sphere : spheres) {
      this->addObject(std::make_shared<Sphere>(sphere));
    }

    for (const auto &polyset : polysets) {
      for (const auto &triangle : polyset.triangles) {
        this->addObject(std::make_shared<Triangle>(triangle));
      }
    }
  };

  AABB getAABB(float t0, float t1) override;

  void addObject(std::shared_ptr<Hittable> object) {
    this->objects.push_back(object);
  };

  std::vector<std::shared_ptr<Hittable>> getLists() const {
    return this->objects;
  };

  // this determins the closest object that the ray hits
  bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
                 float tmax = 10000.f) const override {

    float closest_point = tmax;
    bool any_hit = false;
    HitRecord temp_hit_record;

    for (auto &object : this->objects) {
      bool is_hit =
          object->intersect(ray, temp_hit_record,tmin, closest_point);
      if (is_hit) {
        closest_point = temp_hit_record.t;
        hit_record = temp_hit_record;
        any_hit = true;
      }
    }

    return any_hit;
  }

  // this determins the list of hit object, used for handling transparent
  // objects shadow ray
  std::vector<HitRecord> hit_list(const Ray &ray, float tmin = 0.01f,
                                  float tmax = 10000.f) const {
    std::vector<HitRecord> hit_list;
    for (auto &object : this->objects) {
      HitRecord hit_record;
      bool is_hit = object->intersect(ray, hit_record,tmin, tmax);
      // if there is a hit
      if (is_hit) {
        hit_list.push_back(hit_record);
      }
    }

    // sort the hit list by t
    std::sort(hit_list.begin(), hit_list.end(),
              [](const auto &a, const auto &b) { return a.t < b.t; });

    return hit_list;
  }

  std::shared_ptr<Hittable> operator[](int index) const {
    return this->objects[index];
  };

private:
  std::vector<std::shared_ptr<Hittable>> objects;
};
