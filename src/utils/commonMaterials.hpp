#pragma once
#include "../base/material.hpp"
namespace gl {
static std::shared_ptr<Material> GLASS = std::make_shared<Dielectric>(1.5f);
static std::shared_ptr<Material> MIRROR =
    std::make_shared<Mirror>(vec3(0.8f, 0.8f, 0.8f), 0.0f);
static std::shared_ptr<Material> ROUGH_MIRROR =
    std::make_shared<Mirror>(vec3(1.f), 0.35f);
static std::shared_ptr<Material> PHONG_RED =
    std::make_shared<Phong>(vec3(0.8f, 0.2f, 0.2f),vec3(0.5f),vec3(0.1f),0.5f);
static std::shared_ptr<Material> PHONG_PLASTIC =
    std::make_shared<Phong>(GRAY,vec3(0.2f),vec3(0.5f),0.9f);
static std::shared_ptr<Material> PHONG_GRAY =
    std::make_shared<Phong>(GRAY,vec3(0.5f),vec3(0.f),0.2f);
static std::shared_ptr<Material> LAMBERTIAN_GRAY = std::make_shared<Lambertian>(GRAY);
}; 