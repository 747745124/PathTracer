#pragma once
#include "../base/material.hpp"
#include "../material/conductor.hpp"
#include "../material/dielectric.hpp"
#include "../material/hairMarsch.hpp"
#include "../material/simpleDispersion.hpp"
#include "../material/thinDielectric.hpp"
#include "./colors.hpp"
namespace gl {
static std::shared_ptr<Material> GLASS = std::make_shared<Dielectric>(1.5f);
static std::shared_ptr<Material> THIN_GLASS =
    std::make_shared<ThinDielectric>(1.5f, false);
static std::shared_ptr<Material> THIN_GLASS_SPLIT =
    std::make_shared<ThinDielectric>(1.5f, true);

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
static std::shared_ptr<Material> LAMBERTIAN_RED =
    std::make_shared<Lambertian>(RED);
static std::shared_ptr<Material> LAMBERTIAN_GREEN =
    std::make_shared<Lambertian>(GREEN);
static std::shared_ptr<Material> MARSCH_HAIR = std::make_shared<HairMarschner>(
    vec3(0.419f, 0.697f, 1.37f), 0.f, 1.55f, 0.7f, 0.5f, to_radian(2.f));

static std::shared_ptr<Material> SILVER_MAT = std::make_shared<Conductor>(
    /* eta = */ vec3(0.04f, 0.06f, 0.04f),
    /*   k = */ vec3(4.8f, 3.586f, 2.657f),
    /* alpha_x = */ 0.0001f,
    /* alpha_y = */ 0.0001f);

static std::shared_ptr<Material> GOLD_MAT = std::make_shared<Conductor>(
    /* eta = */ vec3(0.14f, 0.43f, 1.38f),
    /*   k = */ vec3(4.54f, 2.455f, 1.914f),
    /* alpha_x = */ 0.0001f,
    /* alpha_y = */ 0.0001f);

static std::shared_ptr<Material> ROUGH_GOLD_MAT = std::make_shared<Conductor>(
    /* eta = */ vec3(0.14f, 0.43f, 1.38f),
    /*   k = */ vec3(4.54f, 2.455f, 1.914f),
    /* alpha_x = */ 0.1f,
    /* alpha_y = */ 0.1f);

static std::shared_ptr<Material> ROUGH_SILVER_MAT = std::make_shared<Conductor>(
    /* eta = */ vec3(0.04f, 0.06f, 0.04f),
    /*   k = */ vec3(4.8f, 3.586f, 2.657f),
    /* alpha_x = */ 0.1f,
    /* alpha_y = */ 0.1f);

static std::shared_ptr<Material> GLASS_MAT =
    std::make_shared<MFDielectric>(1.5f, 0.0001f, 0.0001f);

static std::shared_ptr<Material> ROUGH_GLASS_MAT =
    std::make_shared<MFDielectric>(1.5f, 0.002f, 0.002f);

static std::shared_ptr<ConstantTexture> DefaultTexture =
    std::make_shared<ConstantTexture>(gl::vec3(1.0f));

static std::shared_ptr<Lambertian> DefaultMaterial =
    std::make_shared<Lambertian>(DefaultTexture);

static std::shared_ptr<DebugNormalMaterial> debugNormalMaterial =
    std::make_shared<DebugNormalMaterial>();
}; // namespace gl
