#pragma once
#include "base/objectList.hpp"
#include "base/primitive.hpp"

class Box : public Hittable
{
public:
    Box(gl::vec3 min_xyz, gl::vec3 max_xyz,
        std::shared_ptr<Material> material = nullptr,
        std::shared_ptr<MediumInterface> medium_interface = nullptr)
        : min_xyz(min_xyz), max_xyz(max_xyz), medium_interface(medium_interface)
    {
        this->objtype = ObjType::BOX_OBJ;

        auto x0 = min_xyz.x();
        auto x1 = max_xyz.x();
        auto y0 = min_xyz.y();
        auto y1 = max_xyz.y();
        auto z0 = min_xyz.z();
        auto z1 = max_xyz.z();

        _faces.addObject(
            std::make_shared<YZRectangle>(x1, y0, y1, z0, z1, material, medium_interface));
        _faces.addObject(
            std::make_shared<YZRectangle>(x0, y0, y1, z0, z1, material, medium_interface));
        _faces.addObject(
            std::make_shared<XZRectangle>(y1, x0, x1, z0, z1, material, medium_interface));
        _faces.addObject(
            std::make_shared<XZRectangle>(y0, x0, x1, z0, z1, material, medium_interface));
        _faces.addObject(
            std::make_shared<XYRectangle>(z1, x0, x1, y0, y1, material, medium_interface));
        _faces.addObject(
            std::make_shared<XYRectangle>(z0, x0, x1, y0, y1, material, medium_interface));
    };

    AABB getAABB(float t0, float t1) override { return AABB(min_xyz, max_xyz); };

    bool intersect(const Ray &ray, HitRecord &hit_record, float tmin = 0.0001,
                   float tmax = 10000.f) const override
    {
        return _faces.intersect(ray, hit_record, tmin, tmax);
    };

    std::shared_ptr<MediumInterface> get_medium_interface() const override
    {
        return this->medium_interface;
    }

    gl::vec3 min_xyz;
    gl::vec3 max_xyz;
    ObjectList _faces;
    std::shared_ptr<MediumInterface> medium_interface;
};