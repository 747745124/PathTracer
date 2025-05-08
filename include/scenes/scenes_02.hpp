#pragma once
#include "base/colors.hpp"
#include "material/commonMaterials.hpp"
#include "render/renderManager.hpp"
SceneInfo cornell_box_base()
{
    using namespace std;
    using namespace gl;

    SceneInfo scene;
    ObjectList objects;
    // used for shadowray
    LightList lights;

    auto red_disneydiffuse = make_shared<DisneyDiffuse>(vec3(0.65, 0.05, 0.05), 0.5f, 0.5f);
    auto red_lambertian = make_shared<Lambertian>(vec3(0.65, 0.05, 0.05));
    auto white = make_shared<DisneyDiffuse>(vec3(0.73f), 0.1f, 1.f);
    auto green = make_shared<DisneyDiffuse>(vec3(0.12, 0.45, 0.15), 0.1f, 1.f);
    auto light = make_shared<DiffuseEmitter>(vec3(1.0f), 15);
    std::array<gl::vec3, 4> vertices;
    vertices[0] = {213, 554, 227};
    vertices[1] = {343, 554, 227};
    vertices[2] = {343, 554, 332};
    vertices[3] = {213, 554, 332};
    lights.addLight(make_shared<QuadLight>(vertices, vec3(1.0f), 15));

    objects.addObject(make_shared<FlipFace>(
        make_shared<AARectangle<Axis::Y>>(554, 213, 343, 227, 332, light)));
    objects.addObject(
        make_shared<AARectangle<Axis::X>>(555, 0, 555, 0, 555, green));
    objects.addObject(make_shared<AARectangle<Axis::X>>(0, 0, 555, 0, 555, red_disneydiffuse));
    objects.addObject(
        make_shared<AARectangle<Axis::Y>>(0, 0, 555, 0, 555, white));
    objects.addObject(
        make_shared<AARectangle<Axis::Y>>(555, 0, 555, 0, 555, white));
    objects.addObject(
        make_shared<AARectangle<Axis::Z>>(555, 0, 555, 0, 555, white));

    scene._height = 300;
    scene._width = 240;
    scene.spp_x = 2;
    scene.spp_y = 2;
    scene._gamma = 1.5;
    scene.camera = make_shared<PerspectiveCamera>(
        gl::to_radian(40.f), (float)(scene._width) / (float)(scene._height), 10.f,
        1000.f, vec3(0, 1, 0), vec3(0, 0, 1.f).normalize(),
        vec3(278.f, 278.f, -800.f));
    scene.objects = objects;
    scene.bg_color = vec3(0.f);
    scene.lights = lights;
    return scene;
};
SceneInfo cornell_box_disneyDiffuse()
{
    using namespace std;
    using namespace gl;

    SceneInfo scene = cornell_box_base();
    shared_ptr<Hittable> sphere_00 =
        make_shared<Sphere>(vec3(80, 250, 190), 50,
                            make_shared<DisneyDiffuse>(vec3(0.65, 0.05, 0.05), 0.f, 0.0f));

    shared_ptr<Sphere> sphere_01 =
        make_shared<Sphere>(vec3(180, 250, 190), 50,
                            make_shared<DisneyDiffuse>(vec3(0.65, 0.05, 0.05), 0.2f, 0.2f));

    shared_ptr<Hittable> sphere_02 =
        make_shared<Sphere>(vec3(280, 250, 190), 50,
                            make_shared<DisneyDiffuse>(vec3(0.65, 0.05, 0.05), 0.4f, 0.4f));

    shared_ptr<Hittable> sphere_03 =
        make_shared<Sphere>(vec3(380, 250, 190), 50,
                            make_shared<DisneyDiffuse>(vec3(0.65, 0.05, 0.05), 0.6f, 0.6f));

    shared_ptr<Hittable> sphere_04 =
        make_shared<Sphere>(vec3(480, 250, 190), 50,
                            make_shared<DisneyDiffuse>(vec3(0.65, 0.05, 0.05), 0.8f, 0.8f));

    scene.objects.addObject(sphere_00);
    scene.objects.addObject(sphere_01);
    scene.objects.addObject(sphere_02);
    scene.objects.addObject(sphere_03);
    scene.objects.addObject(sphere_04);

    return scene;
}

SceneInfo cornell_box_disneyMetal()
{
    using namespace std;
    using namespace gl;

    SceneInfo scene = cornell_box_base();

    shared_ptr<Hittable> sphere_00 =
        make_shared<Sphere>(vec3(80, 250, 190), 50,
                            make_shared<DisneyMetal>(vec3(0.65, 0.05, 0.05), 0.f, 0.8f));

    shared_ptr<Sphere> sphere_01 =
        make_shared<Sphere>(vec3(180, 250, 190), 50,
                            make_shared<DisneyMetal>(vec3(0.65, 0.05, 0.05), 0.2f, 0.8f));

    shared_ptr<Hittable> sphere_02 =
        make_shared<Sphere>(vec3(280, 250, 190), 50,
                            make_shared<DisneyMetal>(vec3(0.65, 0.05, 0.05), 0.4f, 0.8f));

    shared_ptr<Hittable> sphere_03 =
        make_shared<Sphere>(vec3(380, 250, 190), 50,
                            make_shared<DisneyMetal>(vec3(0.65, 0.05, 0.05), 0.6f, 0.8f));

    shared_ptr<Hittable> sphere_04 =
        make_shared<Sphere>(vec3(480, 250, 190), 50,
                            make_shared<DisneyMetal>(vec3(0.65, 0.05, 0.05), 0.8f, 0.8f));

    scene.objects.addObject(sphere_00);
    scene.objects.addObject(sphere_01);
    scene.objects.addObject(sphere_02);
    scene.objects.addObject(sphere_03);
    scene.objects.addObject(sphere_04);

    return scene;
}

SceneInfo cornell_box_disneyClearcoat()
{
    using namespace std;
    using namespace gl;

    SceneInfo scene = cornell_box_base();

    shared_ptr<Hittable> sphere_00 =
        make_shared<Sphere>(vec3(80, 250, 190), 50,
                            make_shared<DisneyClearcoat>(0.2f));

    scene.objects.addObject(sphere_00);

    shared_ptr<Hittable> sphere_01 =
        make_shared<Sphere>(vec3(180, 250, 190), 50,
                            make_shared<DisneyClearcoat>(0.4f));

    scene.objects.addObject(sphere_01);

    shared_ptr<Hittable> sphere_02 =
        make_shared<Sphere>(vec3(280, 250, 190), 50,
                            make_shared<DisneyClearcoat>(0.6f));

    scene.objects.addObject(sphere_02);

    shared_ptr<Hittable> sphere_03 =
        make_shared<Sphere>(vec3(380, 250, 190), 50,
                            make_shared<DisneyClearcoat>(0.8f));

    scene.objects.addObject(sphere_03);

    shared_ptr<Hittable> sphere_04 =
        make_shared<Sphere>(vec3(480, 250, 190), 50,
                            make_shared<DisneyClearcoat>(1.0f));

    scene.objects.addObject(sphere_04);

    return scene;
}