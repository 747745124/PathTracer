#pragma once
#include "./lightList.hpp"
#include "./objectList.hpp"
float attenuate(float distance)
{
    return std::min(1.f, 1.f / (0.25f + 0.1f * distance + 0.01f * distance * distance));
}
// recursive function,where shading happens
gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims, uint max_depth, const LightList &lights)
{
    using namespace gl;

    vec3 color(0.0f, 0.0f, 0.0f);
    vec3 bg(0.0f, 0.0f, 0.0f);

    if (max_depth == 0)
        return vec3(0.0f, 0.0f, 0.0f);

    HitRecord *hit_record = prims.hit(ray);

    // background color
    if (hit_record == nullptr)
        return bg;

    auto hit_point = hit_record->position;
    auto normal = hit_record->normal;
    auto view_dir = -ray.getDirection();
    auto material = hit_record->material;

    auto local_diff = vec3(0.f);
    auto local_spec = vec3(0.f);
    auto local_ambient = vec3(0.f);

    // calculate ambient and diffuse color
    for (const auto &light : lights.get())
    {
        gl::vec3 light_dir(0.0f, 0.0f, 0.0f);

        if (light->type == LightType::POINT_LIGHT)
        {
            auto light_pos = light->position;
            light_dir = (light_pos - hit_point).normalize();
        }
        else if (light->type == LightType::DIRECTIONAL_LIGHT)
        {
            light_dir = static_cast<DirectionalLight *>(light.get())->defaultFront;
            light_dir = -light_dir;
        }

        auto light_color = light->color;

        Ray shadow_ray(hit_point, light_dir);
        HitRecord *shadow_hit_record = prims.hit(shadow_ray);
        if (shadow_hit_record == nullptr)
        {
            auto NoL = std::max(0.f, gl::dot(normal, light_dir));
            auto diffuse = material->diff_color * NoL;

            auto half_dir = (view_dir + light_dir).normalize();
            auto NoH = std::max(0.f, gl::dot(normal, half_dir));
            auto specular = material->spec_color * pow(NoH, material->shininess * 64);

            float attenuation = 1.3f;
            if (light->type == LightType::POINT_LIGHT)
                attenuation = attenuate((light->position - hit_point).length());

            local_diff += attenuation * light_color * diffuse;
            local_spec += attenuation * light_color * specular;
        }
    }

    local_ambient = material->diff_color * material->ambient_color;
    local_ambient *= (1 - material->ktran);
    local_diff *= (1 - material->ktran);

    // if hits a transparent surface, generate a refraction ray
    if (hit_record->material->ktran != 0.f)
    {
        gl::vec3 out_refract_dir = gl::refract(ray.getDirection(), hit_record->normal, 1.5);
        if (out_refract_dir != gl::vec3(0.f))
        {
            // this is used for avoid self-intersection
            Ray out_refract(hit_record->position + 1e-4 * normal, out_refract_dir);
            color += material->ktran * getRayColor(out_refract, prims, max_depth - 1, lights);
        }
    }

    // if hits a specular surface, generate a reflection ray
    if (hit_record->material->spec_color != gl::vec3(0.f))
    {
        gl::vec3 out_reflect_dir = gl::reflect(ray.getDirection(), hit_record->normal);
        // this is used for avoid self-intersection
        Ray out_reflect(hit_record->position + 1e-4 * normal, out_reflect_dir);
        color += material->spec_color.x() * getRayColor(out_reflect, prims, max_depth - 1, lights);
    }

    return color + local_ambient + local_diff + local_spec;
}

// {
//     gl::vec3 light_pos(-1.84647f, 0.778452f, 2.67544f);
//     gl::vec3 light_dir = (light_pos - hit_point).normalize();

//     Ray shadow_ray(hit_point, light_dir);
//     HitRecord *shadow_hit_record = prims.hit(shadow_ray);
//     if (shadow_hit_record == nullptr)
//     {
//         auto normal = hit_record->normal;
//         auto NoL = std::max(0.f, gl::dot(normal, light_dir));
//         color += hit_record->material->diff_color * NoL;
//     }
// }
// {
//     gl::vec3 light_pos(-7.78433f, 2.38677f, -5.08224f);
//     gl::vec3 light_dir = (light_pos - hit_point).normalize();

//     Ray shadow_ray(hit_point, light_dir);
//     HitRecord *shadow_hit_record = prims.hit(shadow_ray);
//     if (shadow_hit_record == nullptr)
//     {
//         auto normal = hit_record->normal;
//         auto NoL = std::max(0.f, gl::dot(normal, light_dir));
//         color += hit_record->material->diff_color * NoL;
//     }
// }
// {
//     gl::vec3 light_dir(0.580339, -0.523277, -0.62401);
//     Ray shadow_ray(hit_point, -light_dir);
//     HitRecord *shadow_hit_record = prims.hit(shadow_ray);
//     if (shadow_hit_record == nullptr)
//     {
//         auto normal = hit_record->normal;
//         auto NoL = std::max(0.f, gl::dot(normal, light_dir));
//         color += hit_record->material->diff_color * NoL;
//     }
// }

// gl::vec3 color(0.0f, 0.0f, 0.0f);
// if (max_depth == 0)
//     return gl::vec3(0.0f, 0.0f, 0.0f);

// HitRecord *hit_record = prims.hit(ray);
// if (hit_record)
// {
//     gl::vec3 target = hit_record->position + hit_record->normal + gl::sphere_random_vec(1.f);
//     Ray next_ray(hit_record->position, target - hit_record->position);
//     return 0.5 * getRayColor(next_ray, prims, max_depth - 1);
// }

// gl::vec3 dir = ray.getDirection().normalize();
// auto t = 0.5 * (dir.y() + 1.0);
// return (1.0 - t) * gl::vec3(1.0, 1.0, 1.0) + t * gl::vec3(0.5, 0.7, 1.0);