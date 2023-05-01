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
    std::shared_ptr<HitRecord> hit(const Ray &ray, float tmin = 0.0001, float tmax = 10000.f) const
    {

        std::shared_ptr<HitRecord> hit_record = nullptr;
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

    // this determins the list of hit object, used for handling transparent objects shadow ray
    std::vector<std::shared_ptr<HitRecord>> hit_list(const Ray &ray, float tmin = 0.0001, float tmax = 10000.f) const
    {
        std::vector<std::shared_ptr<HitRecord>> hit_list;
        for (auto &object : this->objects)
        {
            auto hit_record = object->intersect(ray, tmin, tmax);
            //if there is a hit
            if (hit_record != nullptr)
            {
                hit_list.push_back(hit_record);
            }
        }

        // sort the hit list by t
        std::sort(hit_list.begin(), hit_list.end(), [](const auto &a, const auto &b) {
            return a->t < b->t;
        });

        return hit_list;
    }

private:
    std::vector<std::shared_ptr<Hittable>> objects;
};
