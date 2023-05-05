#pragma once
#include "./object3D.hpp"
#include "../utils/scene_io.hpp"
#include <iostream>
#include <memory>
class Light : public Object3D
{
public:
    Light()
    {
        position = gl::vec3(0.f, 0.f, 0.0f);
        color = gl::vec3(1.0f, 1.0f, 1.0f);
        intensity = 1.0f;
    };

    Light(const gl::vec3 &position, const gl::vec3 &color, float intensity)
    {
        this->color = {1.0f, 1.0f, 1.0f};
        this->intensity = 1.0f;
    }

    ~Light() = default;

    float intensity = 1.0f;
    gl::vec3 color = {1.0f, 1.0f, 1.0f};
    LightType type = LightType::POINT_LIGHT;
};

class PointLight : public Light
{
public:
    PointLight(const gl::vec3 &position, const gl::vec3 &color, float intensity = 1.0f)
    {
        this->type = LightType::POINT_LIGHT;
        this->position = position;
        this->color = color;
        this->intensity = intensity;
    };

    ~PointLight() = default;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(const gl::vec3 &color, const gl::vec3 &direction, float intensity)
    {
        this->color = color;
        this->defaultFront = direction;
        this->intensity = intensity;
        this->position = gl::vec3(0.0f, 0.0f, 0.0f);
        this->type = LightType::DIRECTIONAL_LIGHT;
    };
    
    ~DirectionalLight() = default;
};

class SpotLight : public Light
{
public:
    SpotLight(const gl::vec3 &direction, const gl::vec3 &position, const gl::vec3 &color, float intensity, float cutoff_angle = gl::to_radian(180.0f), float dropoff_rate = 1.0f)
    {
        this->defaultFront = direction;
        this->position = position;
        this->color = color;
        this->intensity = intensity;
        this->cutoff_angle = cutoff_angle;
        this->dropoff_rate = dropoff_rate;
        this->type = LightType::SPOT_LIGHT;
    };

    ~SpotLight() = default;
    float cutoff_angle = gl::to_radian(180.0f); 
    float dropoff_rate = 1.0f;                 
};

using Lights = std::tuple<std::vector<PointLight>,std::vector<DirectionalLight>>;
Lights _get_lights_from_io(const LightIO *io);