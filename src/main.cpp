#include "./unit_test.hpp"
#include "./utils/bvh.hpp"
uint64_t hit_count = 0;

int main() {
    //night_time();
    SceneInfo scene = cornell_box();
    // SceneInfo scene = cornell_box_modified();
    //two_lights();
    // SceneInfo scene = simple_light();
    //night();
    // checkpoint_2();
    //checkpoint_3();
    // SceneInfo scene = VeachMIS();

  // SceneInfo scene = night();

  scene.renderWithInfo("../output.png");
  return 0;
};