#pragma once
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"
#include <cstdint>
#ifdef _WIN32
#include <Windows.h>
using uint = unsigned int;
#endif

class Framebuffer
{
private:
    uint width;
    uint height;
};
