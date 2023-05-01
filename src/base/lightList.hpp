#pragma once
#include "./light.hpp"
// a manager for lights
class LightList
{
public:
    LightList() = default;
    ~LightList() = default;
    LightList(const Lights &lights)
    {
        auto &[p_light, d_light] = lights;
        for (const auto &p : p_light)
        {
            this->addLight(std::make_shared<PointLight>(p));
        }

        for (const auto &d : d_light)
        {
            this->addLight(std::make_shared<DirectionalLight>(d));
        }
    };

    void addLight(std::shared_ptr<Light> object)
    {
        this->lights.push_back(object);
    };

    std::vector<std::shared_ptr<Light>> get() const
    {
        return this->lights;
    }

private:
    std::vector<std::shared_ptr<Light>> lights;
};
