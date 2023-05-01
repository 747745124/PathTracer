#pragma once
#include "./object3D.hpp"
#include "../utils/scene_io.hpp"
#include "./material.hpp"
#include "./ray.hpp"
#include <memory>

struct HitRecord
{
public:
    float t;
    gl::vec3 normal;
    gl::vec3 position;
    std::shared_ptr<CustomMaterial> material;
    // Ref: rt in one weeknd
    // This is used to determine whether the ray is inside or outside the object
    // As we want have the normal always point against the ray
    bool is_inside;
    void set_normal(const Ray &ray, const gl::vec3 &n)
    {
        this->is_inside = dot(ray.getDirection(), n) < 0;
        this->normal = this->is_inside ? n : -n;
    }
};

class Hittable : public Object3D
{
public:
    virtual std::shared_ptr<HitRecord> intersect(const Ray &ray, float tmin, float tmax) const = 0;
    ObjType objtype;
};

class Sphere : public Hittable
{
public:
    float radius;
    gl::vec3 center;
    std::shared_ptr<CustomMaterial> material;
    Sphere(const gl::vec3 &center, float radius) : center(center), radius(radius)
    {
        this->objtype = ObjType::SPHERE_OBJ;
    };

    std::shared_ptr<HitRecord> intersect(const Ray &ray, float tmin = 0.0, float tmax = 10000.f) const override
    {
        auto ray_dir = ray.getDirection().normalize();
        auto ray_origin = ray.getOrigin();
        auto oc = ray_origin - this->center;
        auto a = dot(ray_dir, ray_dir);
        auto b = 2 * dot(ray_dir, oc);
        auto c = dot(oc, oc) - this->radius * this->radius;
        auto discriminant = b * b - 4 * a * c;

        if (discriminant < 0)
        {
            return nullptr;
        }
        else
        {
            auto delta = sqrtf(discriminant);
            auto t = (-b - delta) / (2 * a);
            if (t < tmin || t > tmax)
            {
                t = (-b + delta) / (2 * a);
                if (t < tmin || t > tmax)
                {
                    return nullptr;
                }
            }

            auto hit_record = std::make_shared<HitRecord>();
            hit_record->t = t;
            hit_record->position = ray_origin + t * ray_dir;
            hit_record->set_normal(ray, (hit_record->position - this->center).normalize());
            hit_record->material = this->material;
            return hit_record;
        }
    };
};

class Triangle : public Hittable
{
public:
    Triangle(const gl::vec3 &v0, const gl::vec3 &v1, const gl::vec3 &v2,const CustomMaterial &mat) : v0(v0), v1(v1), v2(v2)
    {
        this->objtype = ObjType::POLYSET_OBJ;
        this->material = std::make_shared<CustomMaterial>(mat);
    };

    gl::vec3 v0, v1, v2;
    std::shared_ptr<CustomMaterial> material;

    std::shared_ptr<HitRecord> intersect(const Ray &ray, float tmin = 0.0, float tmax = 10000.f) const override
    {
        const float epsilon = 1e-6;
        // using Möller–Trumbore intersection algorithm
        auto ray_dir = ray.getDirection().normalize();
        auto ray_origin = ray.getOrigin();
        auto edge1 = this->v1 - this->v0;
        auto edge2 = this->v2 - this->v0;
        auto h = cross(ray_dir, edge2);
        auto a = dot(edge1, h);

        // if isparallel
        if (a > -epsilon && a < epsilon)
            return nullptr;

        auto f = 1 / a;
        auto s = ray_origin - this->v0;
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
        if (t > tmin && t < tmax)
        {
            auto hit_record = std::make_shared<HitRecord>();
            hit_record->t = t;
            hit_record->position = ray_origin + t * ray_dir;
            hit_record->set_normal(ray, cross(edge1, edge2).normalize());
            hit_record->material = this->material;
            return hit_record;
        }
        else
            return nullptr;
    };
};

class PolySet
{
public:
    std::vector<Triangle> triangles;

    PolySet(const std::vector<Triangle> &triangles) : triangles(triangles)
    {
        this->num_polys = triangles.size();
    };

    uint getNumPolys() const
    {
        return this->num_polys;
    }

    PolySetType getType() const
    {
        return this->polytype;
    }

private:
    uint num_polys;
    PolySetType polytype = PolySetType::POLYSET_TRI_MESH;
};

using Primitives = std::tuple<std::vector<Sphere>, std::vector<PolySet>>;
Primitives _get_primitives_from_io(const ObjIO *io);