// #include <windows.h>
#include <stdio.h>
#include "./utils/scene_io.hpp"
#include "./utils/timeit.hpp"
#include "./external/stb_image.h"
#include "./base/framebuffer.hpp"

#define IMAGE_WIDTH 1500
#define IMAGE_HEIGHT 1500

using uchar = unsigned char;

SceneIO *scene = nullptr;

static void loadScene(const char *name)
{
	/* load the scene into the SceneIO data structure using given parsing code */
	scene = readScene(name);

	/* hint: use the Visual Studio debugger ("watch" feature) to probe the
	   scene data structure and learn more about it for each of the given scenes */

	/* write any code to transfer from the scene data structure to your own here */
	/* */

	return;
}

/* just a place holder, feel free to edit */
void render(void)
{
	int i, j, k;
	uchar *image = (uchar *)malloc(sizeof(uchar) * IMAGE_HEIGHT * IMAGE_WIDTH * 3);
	uchar *ptr = image;

	for (j = 0; j < IMAGE_HEIGHT; j++)
	{
		for (i = 0; i < IMAGE_WIDTH; i++)
		{
			for (k = 0; k < 3; k++)
			{
				*(ptr++) = 0;
			}
		}
	}
	/* save out the image */
	/* */

	/* cleanup */
	free(image);

	return;
}

int main(int argc, char *argv[])
{
	std::string file_path = "../Scenes/test1.ascii";
	loadScene(file_path.c_str());

	/* write your ray tracer here */

	auto timeit = make_decorator(render);
	timeit();

	/* cleanup */
	if (scene != nullptr)
	{
		deleteScene(scene);
	}

	return 1;
}
