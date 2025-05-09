#pragma once
#include "disneyGlass.hpp"
#include "disneyMetal.hpp"
#include "disneySheen.hpp"
#include "disneyClearcoat.hpp"
#include "disneyDiffuse.hpp"

class DisneyPrincipledBSDF : public Material
{
private:
    std::shared_ptr<DisneyGlass> glass_mat;
    std::shared_ptr<DisneyMetal> metal_mat;
    std::shared_ptr<DisneySheen> sheen_mat;
    std::shared_ptr<DisneyClearcoat> clearcoat_mat;
    std::shared_ptr<DisneyDiffuse> diffuse_mat;

    float specular_transmission;
    float metallic;
    float clearcoat;

    float diffuse_weight;
    float metal_weight;
    float glass_weight;
    float clearcoat_weight;
    // sheen weight doesn't participate in importance sampling
    float sheen_weight;

    // needed for metal lobe, since it's a variant of the metal lobe
    float specular_tint;
    float eta;
    float specular;

public:
    DisneyPrincipledBSDF(
        const gl::vec3 &baseColor,
        float specular_transmission,
        float metallic,
        float subsurface,
        float specular,
        float roughness,
        float specular_tint,
        float anisotropic,
        float sheen,
        float sheen_tint,
        float clearcoat,
        float clearcoat_gloss,
        float eta)
        : specular_transmission(specular_transmission), metallic(metallic),
          clearcoat(clearcoat), specular_tint(specular_tint), eta(eta), specular(specular)
    {
        glass_mat = std::make_shared<DisneyGlass>(baseColor, roughness, anisotropic, eta);
        metal_mat = std::make_shared<DisneyMetal>(baseColor, roughness, anisotropic);
        sheen_mat = std::make_shared<DisneySheen>(baseColor, sheen_tint);
        clearcoat_mat = std::make_shared<DisneyClearcoat>(clearcoat_gloss);
        diffuse_mat = std::make_shared<DisneyDiffuse>(baseColor, roughness, subsurface);

        // weights for the mixture
        diffuse_weight = (1 - metallic) * (1 - specular_transmission);
        metal_weight = (1 - specular_transmission * (1 - metallic));
        glass_weight = (1 - metallic) * specular_transmission;
        clearcoat_weight = 0.25 * clearcoat;
        sheen_weight = (1 - metallic) * sheen;
    }

    bool scatter(const Ray &ray_in, HitRecord &rec, ScatterRecord &srec,
                 float uc = gl::rand_num(), // coin-flip sample
                 const gl::vec2 &u = {gl::rand_num(),
                                      gl::rand_num()}, // 2D microfacet sample
                 TransportMode mode = TransportMode::Radiance,
                 BxDFReflTransFlags flags = BxDFReflTransFlags::All) const override
    {
        using namespace gl;
        float pdf = 0.0f;
        float weight = 0.0f;
        float weight_sum = 0.0f;

        // if inside, only glass is allowed
        if (rec.is_inside)
        {
            if (glass_mat == nullptr)
                return false;
            return glass_mat->scatter(ray_in, rec, srec, uc, u, mode, flags);
        }

        // 2. Handle non-reflection flags (only transmission allowed)
        if (!(flags & BxDFReflTransFlags::Reflection) && (!!(flags & BxDFReflTransFlags::Transmission)))
        {
            if (glass_mat && specular_transmission > 0.0f && glass_weight > 0.0f)
            {
                return glass_mat->scatter(ray_in, rec, srec, uc, u, mode, flags);
            }
            return false;
        }
        // Pointers to the actual sub-BSDFs corresponding to these weights
        std::vector<std::shared_ptr<Material>> lobes_to_sample = {
            diffuse_mat, metal_mat, glass_mat, clearcoat_mat};

        float pmf_choice;           // Will store the probability of choosing the selected lobe
        float uc_remapped_for_lobe; // The remapped random number for the chosen lobe
        int lobe_index = sampleDiscrete({diffuse_weight, metal_weight, glass_weight, clearcoat_weight}, uc, &pmf_choice, &uc_remapped_for_lobe);

        // No lobe chosen or lobe has zero probability
        if (lobe_index == -1 || pmf_choice <= 1e-6f)
            return false;

        std::shared_ptr<Material> chosen_lobe = lobes_to_sample[lobe_index];
        // non existing lobe
        if (!chosen_lobe)
            return false;

        gl::vec2 u_for_chosen_lobe = {uc_remapped_for_lobe, u[1]};
        bool scattered = chosen_lobe->scatter(ray_in, rec, srec, uc_remapped_for_lobe, u_for_chosen_lobe, mode, flags);

        if (!scattered)
            return false;
        if (srec.is_specular())
            return true;
        if (srec.pdf_val <= 1e-6f)
            return false;

        vec3 wo_world = -ray_in.getDirection().normalize();
        vec3 wi_world = srec.sampled_ray.getDirection().normalize();
        // pdf_val (within single lobe), sampled_type, attenuation (within single lobe) are set by the chosen_lobe
        // we need to override the attenuation for the overall mixture
        srec.attenuation = this->f(wo_world, wi_world, rec, mode);

        // calculate overall PDF
        float pdf_val = 0.f;
        float sum_weights = diffuse_weight + metal_weight + glass_weight + clearcoat_weight;

        // calculate pdf for each lobe (null check)
        if (diffuse_mat)
            pdf_val += ((diffuse_weight / sum_weights) * diffuse_mat->scatter_pdf(wo_world, wi_world, rec, mode, flags));
        if (metal_mat)
            pdf_val += ((metal_weight / sum_weights) * metal_mat->scatter_pdf(wo_world, wi_world, rec, mode, flags));
        if (glass_mat)
            pdf_val += ((glass_weight / sum_weights) * glass_mat->scatter_pdf(wo_world, wi_world, rec, mode, flags));
        if (clearcoat_mat)
            pdf_val += ((clearcoat_weight / sum_weights) * clearcoat_mat->scatter_pdf(wo_world, wi_world, rec, mode, flags));

        // Check the overall mixture PDF
        if (pdf_val <= 1e-7f)
            return false;

        // a fallback pdf for the overall mixture, set for compatibility with the base class
        srec.pdf_ptr = std::make_shared<CosinePDF>(rec.normal);
        return true;
    };

    gl::vec3 f(const gl::vec3 &wo_world, const gl::vec3 &wi_world,
               const HitRecord &rec,
               TransportMode mode = TransportMode::Radiance) const override
    {
        using namespace gl;

        if (rec.is_inside)
            return glass_mat ? glass_mat->f(wo_world, wi_world, rec, mode) : gl::vec3(0.f);

        vec3 result(0.f);
        if (diffuse_mat)
            result += diffuse_mat->f(wo_world, wi_world, rec, mode) * diffuse_weight;
        if (metal_mat)
            result += metal_mat->f_variant(wo_world, wi_world, rec, mode) * metal_weight;
        if (glass_mat)
            result += glass_mat->f(wo_world, wi_world, rec, mode) * glass_weight;
        if (clearcoat_mat)
            result += clearcoat_mat->f(wo_world, wi_world, rec, mode) * clearcoat_weight;
        if (sheen_mat)
            result += sheen_mat->f(wo_world, wi_world, rec, mode) * sheen_weight;
        return result;
    }
};
