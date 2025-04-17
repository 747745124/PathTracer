#pragma once
#include "./light.hpp"
// a manager for lights
class LightList {
public:
  LightList() = default;
  ~LightList() = default;

  LightList(const std::vector<std::shared_ptr<Light>> &lights) {
    this->lights = lights;
  };

  LightList(std::shared_ptr<Light> light) {
    this->lights.clear();
    this->lights.push_back(light);
  };

  void addLight(std::shared_ptr<Light> object) {
    this->lights.push_back(object);
  };

  std::vector<std::shared_ptr<Light>> get() const { return this->lights; }

  std::shared_ptr<Light> get(int index) const { return this->lights[index]; }

  std::shared_ptr<Light> uniform_get() const{
    int index = (int)(gl::rand_num()*lights.size());
    return this->lights[index%lights.size()];
  }

private:
  std::vector<std::shared_ptr<Light>> lights;
};
