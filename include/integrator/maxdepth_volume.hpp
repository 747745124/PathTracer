#pragma once
#include "base/lightList.hpp"
#include "base/objectList.hpp"
#include "config.hpp"
#include "primitives/bvh.hpp"
#include "PDF/hittablePDF.hpp"
#include "PDF/mixedPDF.hpp"
#include "sampler/sampler.hpp"

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            gl::vec3 bg_color, const LightList &lights,
                            uint max_depth = 40,
                            std::shared_ptr<BVHNode> bvh = nullptr, const std::shared_ptr<Medium> &global_medium = nullptr)
{
    using namespace gl;

    HitRecord hit_record;
    bool is_hit = false;
    if (bvh == nullptr)
        is_hit = prims.intersect(ray, hit_record);
    else
        is_hit = bvh->intersect(ray, hit_record);

    if (!is_hit)
        return bg_color;

    gl::vec3 transmittance = gl::vec3(1.f);

    if (global_medium)
    {
        // Get medium properties (for homogeneous, point doesn't matter)
        // Using ray.origin as a dummy point for sample_point
        MediumProperties medium_properties = global_medium->sample_point(ray.getOrigin());
        gl::vec3 sigma_a = medium_properties.sigma_a;

        transmittance = exp(-sigma_a * std::max(0.f, hit_record.t));
    }

    gl::vec3 Le(0.f);
    if (hit_record.material->is_emitter())
        Le = hit_record.material->emit(ray, hit_record);

    return transmittance * Le;
}