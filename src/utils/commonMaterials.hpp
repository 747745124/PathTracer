#pragma once
#include "../base/material.hpp"
#include "../material/hairMarsch.hpp"
namespace gl {
static std::shared_ptr<Material> GLASS = std::make_shared<Dielectric>(1.5f);
static std::shared_ptr<Material> MIRROR =
    std::make_shared<Mirror>(vec3(0.8f, 0.8f, 0.8f), 0.0f);
static std::shared_ptr<Material> ROUGH_MIRROR =
    std::make_shared<Mirror>(vec3(1.f), 0.35f);
static std::shared_ptr<Material> PHONG_RED = std::make_shared<PhongLike>(
    vec3(0.8f, 0.2f, 0.2f), vec3(0.6f), vec3(0.1f), 0.4f, 0.2f);
static std::shared_ptr<Material> PHONG_PLASTIC =
    std::make_shared<PhongLike>(GRAY, vec3(0.2f), vec3(0.5f), 0.5f);
static std::shared_ptr<Material> PHONG_GRAY =
    std::make_shared<PhongLike>(GRAY, vec3(0.5f), vec3(0.f), 0.5f);
static std::shared_ptr<Material> LAMBERTIAN_GRAY =
    std::make_shared<Lambertian>(GRAY);
static std::shared_ptr<Material> MARSCH_HAIR = std::make_shared<HairMarschner>(
    vec3(0.419f, 0.697f, 1.37f), 0.f, 1.55f, 0.7f, 0.5f, to_radian(2.f));
}; // namespace gl