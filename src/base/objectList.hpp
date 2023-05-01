#pragma once
#include "./camera.hpp"
#include "./light.hpp"
#include "./primitive.hpp"
#include "../utils/utility.hpp"
// a manager for hittable objects
class ObjectList
{
public:
    ObjectList() = default;
    ~ObjectList() = default;
    ObjectList(const Primitives &prim)
    {
        auto &[spheres, polysets] = prim;
        for (const auto &sphere : spheres)
        {
            this->addObject(std::make_shared<Sphere>(sphere));
        }

        for (const auto &polyset : polysets)
        {
            for (const auto &triangle : polyset.triangles)
            {
                this->addObject(std::make_shared<Triangle>(triangle));
            }
        }
    };

    void addObject(std::shared_ptr<Hittable> object)
    {
        this->objects.push_back(object);
    };

    // this determins the closest object that the ray hits
    HitRecord *hit(const Ray &ray, float tmin = 0.0, float tmax = 10000.f) const
    {

        HitRecord *hit_record = nullptr;
        float closest_point = tmax;
        for (auto &object : this->objects)
        {
            auto temp_hit_record = object->intersect(ray, tmin, closest_point);
            if (temp_hit_record != nullptr)
            {
                closest_point = temp_hit_record->t;
                hit_record = temp_hit_record;
            }
        }

        return hit_record;
    }

private:
    std::vector<std::shared_ptr<Hittable>> objects;
};

