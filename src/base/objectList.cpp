#include "./objectList.hpp"

// recursive function,where shading happens
gl::vec3 getRayColor(const Ray &ray, const ObjectList &prims, uint max_depth)
{
    gl::vec3 color(0.0f, 0.0f, 0.0f);
    if (max_depth == 0)
        return gl::vec3(0.0f, 0.0f, 0.0f);

    HitRecord *hit_record = prims.hit(ray);
    if (hit_record == nullptr)
        return gl::vec3(0.0f, 0.0f, 0.0f);

    auto hit_point = hit_record->position;
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

    color += hit_record->material->diff_color;
    // if hits a diffuse surface, then return the diffuse color
    if (hit_record->material->spec_color == gl::vec3(0.f))
    {
        return color;
    }

    // if hits a specular surface
    gl::vec3 out_dir = gl::reflect(ray.getDirection(), hit_record->normal);
    Ray out_ray(hit_record->position, out_dir);

    return color + 0.5f * getRayColor(out_ray, prims, max_depth - 1);
}

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