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
};

class Hittable : public Object3D
{
public:
    // virtual void intersect(const Ray &ray, HitRecord &hit_record) const;
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
};

class Triangle : public Hittable
{
public:
    Triangle(const gl::vec3 &v0, const gl::vec3 &v1, const gl::vec3 &v2) : v0(v0), v1(v1), v2(v2){
        this->objtype = ObjType::POLYSET_OBJ;
    };

    gl::vec3 v0, v1, v2;
};

class PolySet
{
public:
    std::vector<Triangle> triangles;
    // assume per object material
    std::shared_ptr<CustomMaterial> material;

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