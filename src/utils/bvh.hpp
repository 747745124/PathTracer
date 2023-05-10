#pragma once
#include "../base/objectList.hpp"
#include "../base/primitive.hpp"
#include "utils/utility.hpp"
#include <algorithm>
#include <cstdlib>

class BVHNode : public Hittable {
public:
  BVHNode() = default;
  BVHNode(ObjectList &object_list, uint i, uint j, float t0 = 0.0f,
          float t1 = 1.0f);
  BVHNode(ObjectList &object_list, float t0=0.0f, float t1=1.0f)
      : BVHNode(object_list, 0, object_list.getLists().size(), t0, t1){};

  std::shared_ptr<HitRecord> intersect(const Ray &ray, float tmin,
                                       float tmax) const override;

  AABB getAABB(float t0, float t1) override { return this->box; };

  AABB box;
  std::shared_ptr<Hittable> left;
  std::shared_ptr<Hittable> right;
};

inline std::shared_ptr<HitRecord> BVHNode::intersect(const Ray &ray, float tmin,
                                                     float tmax) const {
  if (!this->box.intersect(ray, tmin, tmax)) {
    return nullptr;
  }

  auto left_hit = this->left->intersect(ray, tmin, tmax);
  auto right_hit = this->right->intersect(ray, tmin, tmax);

  if (left_hit == nullptr && right_hit == nullptr)
    return nullptr;
  if (left_hit == nullptr)
    return right_hit;
  if (right_hit == nullptr)
    return left_hit;
  if (left_hit->t < right_hit->t)
    return left_hit;

  return right_hit;
};

// randomly choose an axis
// sort the primitives
// put half in each subtree
inline BVHNode::BVHNode(ObjectList &object_list, uint i, uint j, float t0,
                        float t1) {

  int axis = gl::C_rand_int(0, 3);
  auto list_copy = object_list.getLists();

  auto compare = [&](std::shared_ptr<Hittable> p1, std::shared_ptr<Hittable> p2,
                     int axis_index) {
    return p1->getAABB(t0, t1).get_min()[axis_index] <
           p2->getAABB(t0, t1).get_min()[axis_index];
  };

  // single element
  if (j - i == 1) {
    this->left = list_copy[i];
    this->right = list_copy[i];
    // 2 elements, compare the min
  } else if (j - i == 2) {

    if (compare(list_copy[i], list_copy[i + 1], axis)) {
      this->left = list_copy[i];
      this->right = list_copy[i + 1];
    } else {
      this->left = list_copy[i + 1];
      this->right = list_copy[i];
    }

    // 3 elements, sort and divide
  } else {
    std::sort(list_copy.begin() + i, list_copy.begin() + j,
              [&](std::shared_ptr<Hittable> p1, std::shared_ptr<Hittable> p2) {
                return compare(p1, p2, axis);
              });

    uint mid = i + (j - i) / 2;
    this->left = std::make_shared<BVHNode>(object_list, i, mid, t0, t1);
    this->right = std::make_shared<BVHNode>(object_list, mid, j, t0, t1);
  }

  this->box =
      AABB::merge(this->left->getAABB(t0, t1), this->right->getAABB(t0, t1));
};
