#include "./unit_test.hpp"
#include "./utils/bvh.hpp"
uint64_t hit_count = 0;

int main() {
    //night_time();
    //cornell_box();
    //cornell_box_modified();
    //two_lights();
    //simple_light();
    //night();
    //checkpoint_2();
    //checkpoint_3();
    //VeachMIS();

  SceneInfo scene = simple_light();

  scene.renderWithInfo("../output.png");
  return 0;
};