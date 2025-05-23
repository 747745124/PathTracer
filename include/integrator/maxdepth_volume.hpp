#pragma once
#include "base/lightList.hpp"
#include "base/objectList.hpp"
#include "config.hpp"
#include "primitives/bvh.hpp"
#include "PDF/hittablePDF.hpp"
#include "PDF/mixedPDF.hpp"
#include "sampler/sampler.hpp"

inline gl::vec3 estimate_l_scatter1(const gl::vec3 &p_scatter,
                                    const gl::vec3 &wo_world,
                                    const std::shared_ptr<Medium> &medium,
                                    const Hittable &prims,
                                    const LightList &lights,
                                    std::shared_ptr<BVHNode> bvh)
{
    using namespace gl;
    if (!medium)
        return gl::vec3(0.0f);

    MediumProperties medium_props = medium->sample_properties_at(p_scatter);
    if (!medium_props.phase_function)
        return gl::vec3(0.0f);

    gl::vec3 L_scatter1_accum(0.0f);
    if (lights.size() > 0 && LIGHT_SAMPLE_NUM > 0)
    {
        for (int i = 0; i < LIGHT_SAMPLE_NUM; i++) // Loop for Number of Direct Light Samples
        {
            auto light = lights.uniform_get(); // Uniformly pick one light from the list
            if (!light)
                continue;

            auto p_prime = light->get_sample(rand_num(), rand_num());
            auto n_prime = light->get_normal_at(p_prime);
            auto dir_prime_to_scatter = normalize(p_scatter - p_prime);
            auto distance_prime_to_scatter = (p_scatter - p_prime).length();

            Ray shadow_ray(p_prime, dir_prime_to_scatter);
            HitRecord shadow_hit_record;
            bool is_shadow_hit = false;
            is_shadow_hit = bvh ? bvh->intersect(shadow_ray, shadow_hit_record, 0.001f, distance_prime_to_scatter - 0.001f)
                                : prims.intersect(shadow_ray, shadow_hit_record, 0.001f, distance_prime_to_scatter - 0.001f);
            auto V = is_shadow_hit ? 0.0f : 1.0f;

            if (V > 0.0f)
            {
                HitRecord light_surface_hit; // HitRecord on the light's surface for L_emit
                light_surface_hit.position = p_prime;
                light_surface_hit.normal = n_prime;
                // CRITICAL ASSUMPTION: uniform light color
                light_surface_hit.texCoords = vec2(0.5f, 0.5f);
                auto Le_prime = light->L_emit(light_surface_hit, dir_prime_to_scatter);
                auto pdf_light = 1.f / (light->get_area() * lights.size());
                auto cos_theta = absDot(n_prime, dir_prime_to_scatter);
                // p is defined as both direction pointing outwards
                auto phase_val = medium_props.phase_function->p(wo_world, -dir_prime_to_scatter);
                auto transmittance = exp(-medium_props.sigma_t() * distance_prime_to_scatter);

                if (distance_prime_to_scatter < gl::epsilon)
                    continue;
                L_scatter1_accum += (Le_prime * cos_theta * phase_val * transmittance) / pdf_light / square(distance_prime_to_scatter);
            }
        }

        L_scatter1_accum /= LIGHT_SAMPLE_NUM;
    }

    return L_scatter1_accum;
}

inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
                            gl::vec3 bg_color, const LightList &lights,
                            uint max_depth = 40,
                            std::shared_ptr<BVHNode> bvh = nullptr, const std::shared_ptr<Medium> &global_medium = nullptr)
{
    using namespace gl;

    HitRecord hit_record;
    bool is_hit = false;

    if (!global_medium)
        return vec3(0.f);
    if (bvh == nullptr)
        is_hit = prims.intersect(ray, hit_record);
    else
        is_hit = bvh->intersect(ray, hit_record);

    MediumProperties medium_properties = global_medium->sample_properties_at(ray.getOrigin());
    float u = halton_sampler.get1D();
    float sigma_t = maxComponent(medium_properties.sigma_t());
    float t = -log(1 - u) / sigma_t;

    if (t < hit_record.t)
    {

        // float trans_pdf = exp(-sigma_t * t) * sigma_t;
        // float transmittance = exp(-sigma_t * t);
        vec3 p = ray.at(t);
        vec3 L_s1_estimate = estimate_l_scatter1(p, -ray.getDirection().normalize(), global_medium, prims, lights, bvh);
        vec3 albedo = medium_properties.sigma_s / medium_properties.sigma_t();

        return albedo * L_s1_estimate / sigma_t;
    }
    // Surface interaction occurs first (or at the same distance, or ray escapes to infinity if no hit)
    else
    {
        // float trans_pdf = exp(-sigma_t * hit_record.t);
        // vec3 transmittance = exp(-sigma_t * hit_record.t);
        vec3 Le(0.f);

        if (hit_record.material != nullptr && hit_record.material->is_emitter())
            Le = hit_record.material->emit(ray, hit_record);

        return Le;
    }
}

// inline gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims,
//                             gl::vec3 bg_color, const LightList &lights,
//                             uint max_depth = 40,
//                             std::shared_ptr<BVHNode> bvh = nullptr, const std::shared_ptr<Medium> &global_medium = nullptr)
// {
//     using namespace gl;

//     HitRecord hit_record;
//     bool is_hit = false;
//     if (bvh == nullptr)
//         is_hit = prims.intersect(ray, hit_record);
//     else
//         is_hit = bvh->intersect(ray, hit_record);

//     if (!is_hit)
//         return bg_color;

//     gl::vec3 transmittance = gl::vec3(1.f);

//     if (global_medium)
//     {
//         // Get medium properties (for homogeneous, point doesn't matter)
//         // Using ray.origin as a dummy point for sample_point
//         MediumProperties medium_properties = global_medium->sample_properties_at(ray.getOrigin());
//         gl::vec3 sigma_a = medium_properties.sigma_a;

//         transmittance = exp(-sigma_a * std::max(0.f, hit_record.t));
//     }

//     gl::vec3 Le(0.f);
//     if (hit_record.material->is_emitter())
//         Le = hit_record.material->emit(ray, hit_record);

//     return transmittance * Le;
// }