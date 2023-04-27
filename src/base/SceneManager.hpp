#pragma once
#include "./camera.hpp"
#include "./light.hpp"

class SceneManager {
    std::unique_ptr<PerspectiveCamera> _camera;
    std::vector<std::unique_ptr<Light>> _lights;
    

};