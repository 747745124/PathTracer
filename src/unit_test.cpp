#include "./unit_test.hpp"
#define FRAMEBUFFER_TEST
// #define LOAD_TEST

int main()
{
#ifdef LOAD_TEST
    std::string name = "../Scenes/test1.ascii";
    auto scene = readScene(name);
    auto camera = PerspectiveCamera(scene->camera);
    auto [plights, dlights] = _get_lights_from_io(scene->lights);
    auto [spheres, polysets] = _get_primitives_from_io(scene->objects);
#endif

#ifdef FRAMEBUFFER_TEST
    FrameBuffer fb(1500, 1500, 3);
    for (int i = 0; i < 1500; i++)
    {
        for (int j = 0; j < 1500; j++)
        {
            fb.setPixelColor(i, j, gl::vec3(1.0f, 0.0f, 0.0f));
        }
    }
    fb.writeToFile("../test.png");

#endif
}