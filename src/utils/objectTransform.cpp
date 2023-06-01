#include "./objectTransform.hpp"
template <> AABB Rotate<Axis::X>::getAABB(float t0, float t1) {
  using namespace gl;
  using namespace std;

  vec3 min(MAXFLOAT);
  vec3 max(-MAXFLOAT);

  auto box = object->getAABB(t0, t1);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        auto x = i * box.get_max().x() + (1 - i) * box.get_min().x();
        auto y = j * box.get_max().y() + (1 - j) * box.get_min().y();
        auto z = k * box.get_max().z() + (1 - k) * box.get_min().z();

        auto new_yz = rotation2D(angle) * vec2(y, z);
        vec3 temp(x, new_yz[0], new_yz[1]);

        for (int c = 0; c < 3; c++) {
          min[c] = std::min(min[c], temp[c]);
          max[c] = std::max(max[c], temp[c]);
        }
      }
    }
  }

  return AABB(min, max);
};

template <> AABB Rotate<Axis::Y>::getAABB(float t0, float t1) {
  using namespace gl;
  using namespace std;

  vec3 min(MAXFLOAT);
  vec3 max(-MAXFLOAT);

  auto box = object->getAABB(t0, t1);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        auto x = i * box.get_max().x() + (1 - i) * box.get_min().x();
        auto y = j * box.get_max().y() + (1 - j) * box.get_min().y();
        auto z = k * box.get_max().z() + (1 - k) * box.get_min().z();

        auto new_xz = rotation2D(angle) * vec2(x, z);
        vec3 temp(new_xz[0], y, new_xz[1]);

        for (int c = 0; c < 3; c++) {
          min[c] = std::min(min[c], temp[c]);
          max[c] = std::max(max[c], temp[c]);
        }
      }
    }
  }

  return AABB(min, max);
};

template <> AABB Rotate<Axis::Z>::getAABB(float t0, float t1) {
  using namespace gl;
  using namespace std;

  vec3 min(MAXFLOAT);
  vec3 max(-MAXFLOAT);

  auto box = object->getAABB(t0, t1);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        auto x = i * box.get_max().x() + (1 - i) * box.get_min().x();
        auto y = j * box.get_max().y() + (1 - j) * box.get_min().y();
        auto z = k * box.get_max().z() + (1 - k) * box.get_min().z();

        auto new_xy = rotation2D(angle) * vec2(x, y);
        vec3 temp(new_xy[0], new_xy[1], z);

        for (int c = 0; c < 3; c++) {
          min[c] = std::min(min[c], temp[c]);
          max[c] = std::max(max[c], temp[c]);
        }
      }
    }
  }

  return AABB(min, max);
};

template <>
bool Rotate<Axis::X>::intersect(const Ray &ray, HitRecord &hit_record,
                                float tmin, float tmax) const {

  auto ray_dir = ray.getDirection().normalize();
  auto ray_origin = ray.getOrigin();

  ray_origin[1] =
      cos_theta * ray.getOrigin()[1] - sin_theta * ray.getOrigin()[2];
  ray_origin[2] =
      sin_theta * ray.getOrigin()[1] + cos_theta * ray.getOrigin()[2];

  ray_dir[1] =
      cos_theta * ray.getDirection()[1] - sin_theta * ray.getDirection()[2];
  ray_dir[2] =
      sin_theta * ray.getDirection()[1] + cos_theta * ray.getDirection()[2];

  Ray rotated_ray(ray_origin, ray_dir);
  if (!object->intersect(rotated_ray, hit_record, tmin, tmax)) {
    return false;
  }

  auto position = hit_record.position;
  auto normal = hit_record.normal;

  position[1] =
      cos_theta * hit_record.position[1] + sin_theta * hit_record.position[2];
  position[2] =
      -sin_theta * hit_record.position[1] + cos_theta * hit_record.position[2];

  normal[1] =
      cos_theta * hit_record.normal[1] + sin_theta * hit_record.normal[2];
  normal[2] =
      -sin_theta * hit_record.normal[1] + cos_theta * hit_record.normal[2];

  hit_record.position = position;
  hit_record.set_normal(rotated_ray, hit_record.normal);

  return true;
}

template <>
bool Rotate<Axis::Y>::intersect(const Ray &ray, HitRecord &hit_record,
                                float tmin, float tmax) const {

  auto ray_dir = ray.getDirection().normalize();
  auto ray_origin = ray.getOrigin();

  ray_origin[0] =
      cos_theta * ray.getOrigin()[0] + sin_theta * ray.getOrigin()[2];
  ray_origin[2] =
      -sin_theta * ray.getOrigin()[0] + cos_theta * ray.getOrigin()[2];

  ray_dir[0] =
      cos_theta * ray.getDirection()[0] + sin_theta * ray.getDirection()[2];
  ray_dir[2] =
      -sin_theta * ray.getDirection()[0] + cos_theta * ray.getDirection()[2];

  Ray rotated_ray(ray_origin, ray_dir);
  if (!object->intersect(rotated_ray, hit_record, tmin, tmax)) {
    return false;
  }

  auto position = hit_record.position;
  auto normal = hit_record.normal;

  position[0] =
      cos_theta * hit_record.position[0] - sin_theta * hit_record.position[2];
  position[2] =
      sin_theta * hit_record.position[0] + cos_theta * hit_record.position[2];

  normal[0] =
      cos_theta * hit_record.normal[0] - sin_theta * hit_record.normal[2];
  normal[2] =
      sin_theta * hit_record.normal[0] + cos_theta * hit_record.normal[2];

  hit_record.position = position;
  hit_record.set_normal(rotated_ray, hit_record.normal);
  return true;
}

template <>
bool Rotate<Axis::Z>::intersect(const Ray &ray, HitRecord &hit_record,
                                float tmin, float tmax) const {

  auto ray_dir = ray.getDirection().normalize();
  auto ray_origin = ray.getOrigin();

  ray_origin[0] =
      cos_theta * ray.getOrigin()[0] - sin_theta * ray.getOrigin()[1];
  ray_origin[1] =
      sin_theta * ray.getOrigin()[0] + cos_theta * ray.getOrigin()[1];

  ray_dir[0] =
      cos_theta * ray.getDirection()[0] - sin_theta * ray.getDirection()[1];
  ray_dir[1] =
      sin_theta * ray.getDirection()[0] + cos_theta * ray.getDirection()[1];

  Ray rotated_ray(ray_origin, ray_dir);
  if (!object->intersect(rotated_ray, hit_record, tmin, tmax)) {
    return false;
  }

  auto position = hit_record.position;
  auto normal = hit_record.normal;

  position[0] =
      cos_theta * hit_record.position[0] + sin_theta * hit_record.position[1];
  position[1] =
      -sin_theta * hit_record.position[0] + cos_theta * hit_record.position[1];

  normal[0] =
      cos_theta * hit_record.normal[0] + sin_theta * hit_record.normal[1];
  normal[1] =
      -sin_theta * hit_record.normal[0] + cos_theta * hit_record.normal[1];

  hit_record.position = position;
  hit_record.set_normal(rotated_ray, hit_record.normal);
  return true;
}
