#include <stdio.h>
#include "./utils/timeit.hpp"
#include "./unit_test.hpp"

#define IMAGE_WIDTH 1500
#define IMAGE_HEIGHT 1500
#define SPP_X 2
#define SPP_Y 2
#define MAX_DEPTH 10
#define GAMMA 1.0f
#define GL_SIMD

using uchar = unsigned char;
const char* output_file = "../scene3.png";
const char* input_file = "../Scenes/test3.scene";

SceneIO *scene = nullptr;
std::unique_ptr<PerspectiveCamera> camera = nullptr;
LightList lights;
ObjectList prims;

static void loadScene(const char *name)
{
	/* load the scene into the SceneIO data structure using given parsing code */
	scene = readScene(name);
	camera.reset(new PerspectiveCamera(scene->camera, (float)IMAGE_WIDTH / (float)IMAGE_HEIGHT));
	lights = LightList(_get_lights_from_io(scene->lights));
	prims = ObjectList(_get_primitives_from_io(scene->objects));
	return;
}

/* just a place holder, feel free to edit */
void render(void)
{
	using namespace gl;
	FrameBuffer fb(IMAGE_WIDTH, IMAGE_HEIGHT, 3, SPP_X, SPP_Y);
	auto offsets = fb.getOffsets();
	uint counter = 0;

#pragma omp parallel for num_threads(omp_get_num_procs() + 1)
	{
		for (int i = 0; i < IMAGE_WIDTH; i++)
		{
			std::cout << "Now scanning " << (float(counter) / IMAGE_WIDTH) * 100.f << " %" << std::endl;

			for (int j = 0; j < IMAGE_HEIGHT; j++)
			{
				auto color = vec3(0.0);
				for (int k = 0; k < fb.getSampleCount(); k++)
				{
					auto sample_color = vec3(0.0);
					vec2 uv = (vec2(i, j) + offsets[k]) / vec2(IMAGE_WIDTH, IMAGE_HEIGHT);
					Ray ray = camera->generateRay(uv);
					color += getRayColor(ray, prims, MAX_DEPTH, lights);
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
	fb.writeToFile(output_file, GAMMA);
	return;
}

int main(int argc, char *argv[])
{
	std::string file_path = input_file;
	loadScene(file_path.c_str());


	auto timeit = make_decorator(render);
	timeit();

	/* cleanup */
	if (scene != nullptr)
	{
		deleteScene(scene);
	}

	return 0;
}
