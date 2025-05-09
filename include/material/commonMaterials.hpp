#pragma once
#include "base/colors.hpp"
#include "material/conductor.hpp"
#include "material/dielectric.hpp"
#include "material/hairMarsch.hpp"
#include "material/simpleDispersion.hpp"
#include "material/thinDielectric.hpp"
#include "material/disneyPrincipledBSDF.hpp"
namespace gl
{
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
        std::make_shared<MFDielectric>(1.5f, 0.3f, 0.3f);

    static std::shared_ptr<ConstantTexture> DefaultTexture =
        std::make_shared<ConstantTexture>(gl::vec3(1.0f));

    static std::shared_ptr<Lambertian> DefaultMaterial =
        std::make_shared<Lambertian>(DefaultTexture);

    static std::shared_ptr<DebugNormalMaterial> debugNormalMaterial =
        std::make_shared<DebugNormalMaterial>();

    namespace DisneyBSDF
    {
        static std::shared_ptr<DisneyDiffuse> DisneyDiffuseRed =
            std::make_shared<DisneyDiffuse>(RED, 0.5f, 0.5f);

        static std::shared_ptr<DisneyMetal> DisneyMetalRed =
            std::make_shared<DisneyMetal>(RED, 0.5f, 0.01f);

        static std::shared_ptr<DisneyClearcoat> Disneycoat =
            std::make_shared<DisneyClearcoat>(0.5f);

        static std::shared_ptr<DisneyPrincipledBSDF> MatteRedPlastic =
            std::make_shared<DisneyPrincipledBSDF>(
                gl::vec3(0.8f, 0.1f, 0.1f), // baseColor
                0.0f,                       // specular_transmission
                0.0f,                       // metallic
                0.0f,                       // subsurface
                0.5f,                       // specular
                0.6f,                       // roughness
                0.0f,                       // specular_tint
                0.0f,                       // anisotropic
                0.0f,                       // sheen
                0.0f,                       // sheen_tint
                0.0f,                       // clearcoat
                0.0f,                       // clearcoat_gloss
                1.460f                      // eta
            );

        static std::shared_ptr<DisneyPrincipledBSDF> GlossyBluePlastic =
            std::make_shared<DisneyPrincipledBSDF>(
                gl::vec3(0.1f, 0.2f, 0.8f), // baseColor
                0.0f,                       // specular_transmission
                0.0f,                       // metallic
                0.0f,                       // subsurface
                0.5f,                       // specular
                0.1f,                       // roughness
                0.0f,                       // specular_tint
                0.0f,                       // anisotropic
                0.0f,                       // sheen
                0.0f,                       // sheen_tint
                0.0f,                       // clearcoat
                0.0f,                       // clearcoat_gloss
                1.5f                        // eta
            );

        static std::shared_ptr<DisneyPrincipledBSDF> ClearGlass =
            std::make_shared<DisneyPrincipledBSDF>(
                gl::vec3(1.0f, 1.0f, 1.0f), // baseColor (often less important for pure glass)
                1.0f,                       // specular_transmission
                0.0f,                       // metallic
                0.0f,                       // subsurface
                0.5f,                       // specular
                0.0f,                       // roughness
                0.0f,                       // specular_tint
                0.0f,                       // anisotropic
                0.0f,                       // sheen
                0.0f,                       // sheen_tint
                0.0f,                       // clearcoat
                0.0f,                       // clearcoat_gloss
                1.52f                       // eta
            );

        static std::shared_ptr<DisneyPrincipledBSDF> Gold =
            std::make_shared<DisneyPrincipledBSDF>(
                gl::vec3(1.0f, 0.766f, 0.336f), // baseColor (defines reflection color for metals)
                0.0f,                           // specular_transmission
                1.0f,                           // metallic
                0.0f,                           // subsurface
                1.0f,                           // specular (intensity, baseColor provides tint)
                0.2f,                           // roughness
                0.0f,                           // specular_tint (not used for metals as baseColor is the tint)
                0.0f,                           // anisotropic
                0.0f,                           // sheen
                0.0f,                           // sheen_tint
                0.0f,                           // clearcoat
                0.0f,                           // clearcoat_gloss
                0.47f                           // eta (real part of IOR for gold, role depends on metallic path)
            );

        static std::shared_ptr<DisneyPrincipledBSDF> MetallicRedCarPaint =
            std::make_shared<DisneyPrincipledBSDF>(
                gl::vec3(0.7f, 0.05f, 0.05f), // baseColor for metallic flakes
                0.0f,                         // specular_transmission
                0.75f,                        // metallic (for the flake layer)
                0.0f,                         // subsurface
                0.6f,                         // specular for base
                0.3f,                         // roughness for metallic flake layer
                0.9f,                         // specular_tint for base if metallic < 1
                0.0f,                         // anisotropic
                0.0f,                         // sheen
                0.0f,                         // sheen_tint
                1.0f,                         // clearcoat strength
                0.95f,                        // clearcoat_gloss (high gloss)
                1.5f                          // eta (for base paint layer, clearcoat often assumes ~1.5)
            );
    };
}; // namespace gl
