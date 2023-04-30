#include "./unit_test.hpp"
// #define FRAMEBUFFER_TEST
// #define LOAD_TEST
// #define BASIC_SPHERE
// #define BASIC_POLYSETS
#define OBJECT_LIST

int main()
{

#ifdef OBJECT_LIST
    std::string name = "../Scenes/test1.ascii";
    auto scene = readScene(name.c_str());
    auto camera = PerspectiveCamera(scene->camera, 1.33);
    auto [plights, dlights] = _get_lights_from_io(scene->lights);
    ObjectList prims(_get_primitives_from_io(scene->objects));

    uint width = 400, height = 300;
    FrameBuffer fb(width, height, 3);

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Ray ray = camera.generateRay(i / float(width), j / float(height));
            auto is_hit = prims.hit(ray);
            if (is_hit!=nullptr)
            {   
                auto base_color = is_hit->material->diff_color;
                fb.setPixelColor(j, i, base_color);
            }
        }
    }

    fb.writeToFile("../test.png");
#endif

#ifdef LOAD_TEST
    std::string name = "../Scenes/test1.ascii";
    auto scene = readScene(name);
    auto camera = PerspectiveCamera(scene->camera);
    auto [plights, dlights] = _get_lights_from_io(scene->lights);
    auto [spheres, polysets] = _get_primitives_from_io(scene->objects);
#endif

#ifdef BASIC_POLYSETS
    std::string name = "../Scenes/test1.ascii";
    auto scene = readScene(name.c_str());
    auto camera = PerspectiveCamera(scene->camera, 1.33);
    // auto camera = PerspectiveCamera(gl::to_radian(45), 1.33, 1, gl::vec3(0, 1, 0), gl::vec3(0, 0, -1), gl::vec3(0, 0, 0));
    auto [plights, dlights] = _get_lights_from_io(scene->lights);
    auto [spheres, polysets] = _get_primitives_from_io(scene->objects);

    uint width = 400, height = 300;
    FrameBuffer fb(width, height, 3);

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Ray ray = camera.generateRay(i / float(width), j / float(height));
            for (const auto &polyset : polysets)
            {
                for (const auto &tri : polyset.triangles)
                {
                    if (tri.intersect(ray))
                    {
                        fb.setPixelColor(j, i, gl::vec3(1, 0, 0));
                    }
                }
            }

            for (const auto &sphere : spheres)
            {
                if (sphere.intersect(ray))
                {
                    fb.setPixelColor(j, i, gl::vec3(0, 1, 0));
                }
            }
        }
    }

    fb.writeToFile("../test.png");
#endif

#ifdef BASIC_SPHERE
    std::string name = "../Scenes/test2.ascii";
    auto scene = readScene(name.c_str());
    auto camera = PerspectiveCamera(scene->camera, 1.33);
    // auto camera = PerspectiveCamera(gl::to_radian(45), 1.33, 1, gl::vec3(0, 1, 0), gl::vec3(0, 0, -1), gl::vec3(0, 0, 0));
    auto [plights, dlights] = _get_lights_from_io(scene->lights);
    auto [spheres, polysets] = _get_primitives_from_io(scene->objects);
    uint width = 400, height = 300;
    FrameBuffer fb(width, height, 3);

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Ray ray = camera.generateRay(i / float(width), j / float(height));
            for (const auto &sphere : spheres)
            {
                if (sphere.intersect(ray))
                {
                    fb.setPixelColor(j, i, gl::vec3(1.0f, 0.0f, 0.0f));
                }
            }
        }
    }

    fb.writeToFile("../test.png");
#endif

#ifdef FRAMEBUFFER_TEST
    FrameBuffer fb(2000, 1500, 3);
    for (int i = 0; i < 2000; i++)
    {
        for (int j = 0; j < 1500; j++)
        {
            fb.setPixelColor(0 + 100, i, gl::vec3(1.0f, 0.0f, 0.0f));
        }
    }
    fb.writeToFile("../test.png");
#endif
}