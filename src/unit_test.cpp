#include "./unit_test.hpp"
// #define FRAMEBUFFER_TEST
// #define LOAD_TEST
// #define BASIC_SPHERE
// #define BASIC_POLYSETS
// #define OBJECT_LIST
// #define RECURSIVE_TEST
// #define SIMD_TEST

int main()
{

#ifdef SIMD_TEST
    using namespace gl;
    vec4 a(0.0f, 0.0f, 1.0f, 1.0f);
    vec4 b(0.0, 0.0f, 1.0f, 1.0f);
    std::cout << a.normalize() << std::endl;
#endif

#ifdef RECURSIVE_TEST
    using namespace gl;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> duration;
    start = std::chrono::system_clock::now();

    std::string name = "../Scenes/test3.ascii";
    auto scene = readScene(name.c_str());
    auto camera = PerspectiveCamera(scene->camera, 1);
    LightList lights(_get_lights_from_io(scene->lights));
    ObjectList prims(_get_primitives_from_io(scene->objects));

    uint width = 1500, height = 1500;
    FrameBuffer fb(width, height, 3, 4, 4);
    auto offsets = fb.getOffsets();
    uint counter = 0;

#pragma omp parallel for num_threads(omp_get_num_procs() + 1)
    {
        for (int i = 0; i < width; i++)
        {
            std::cout << "Now scanning " << (float(counter) / width) * 100.f << " %" << std::endl;

            for (int j = 0; j < height; j++)
            {
                auto color = vec3(0.0);
                for (int k = 0; k < fb.getSampleCount(); k++)
                {
                    auto sample_color = vec3(0.0);
                    vec2 uv = (vec2(i, j) + offsets[k]) / vec2(width, height);
                    Ray ray = camera.generateRay(uv);
                    color += getRayColor(ray, prims, 5u, lights);
                }

// implicit barrier at this section
#pragma omp critical
                {
                    color /= fb.getSampleCount();
                    fb.setPixelColor(j, i, color);
                }
            }

            counter++;
        }
    }

    // fb.gaussianBlur(3, 1.0f);
    fb.writeToFile("../test2.png", 1.0f);

    end = std::chrono::system_clock::now();
    duration = end - start;
    std::cout << duration.count() << " seconds" << std::endl;
#endif

#ifdef OBJECT_LIST
    using namespace gl;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> duration;
    start = std::chrono::system_clock::now();

    std::string name = "../Scenes/test1.ascii";
    auto scene = readScene(name.c_str());
    auto camera = PerspectiveCamera(scene->camera, 1.33);
    auto [plights, dlights] = _get_lights_from_io(scene->lights);
    ObjectList prims(_get_primitives_from_io(scene->objects));

    uint width = 400, height = 300;
    FrameBuffer fb(width, height, 3, 5, 5);
    auto offsets = fb.getOffsets();

#pragma omp parallel for
    {
        for (int i = 0; i < width; i++)
        {
            std::cout << "Now scanning" << i << " of " << width << std::endl;

            for (int j = 0; j < height; j++)
            {
                auto color = vec3(0.0);
                for (int k = 0; k < fb.getSampleCount(); k++)
                {
                    auto sample_color = vec3(0.0);
                    vec2 uv = (vec2(i, j) + offsets[k]) / vec2(width, height);
                    Ray ray = camera.generateRay(uv);
                    auto is_hit = prims.hit(ray);
                    if (is_hit != nullptr)
                    {
                        auto base_color = is_hit->material->diff_color;
                        color += base_color;
                    }
                }

// implicit barrier at this section
#pragma omp critical
                {
                    color /= fb.getSampleCount();
                    fb.setPixelColor(j, i, color);
                }
            }
        }
    }

    fb.writeToFile("../test.png");

    end = std::chrono::system_clock::now();
    duration = end - start;
    std::cout << duration.count() << " seconds" << std::endl;
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