#pragma once
#include "./utils/matrix.hpp"

#define LIGHT_SAMPLE_X 3
#define LIGHT_SAMPLE_Y 3
#define MAX_RAY_DEPTH 15
#define SPP_X 30
#define SPP_Y 30
#define GAMMA 2.0
#define WIDTH 100
#define HEIGHT 100
#define BG_COLOR gl::vec3(0.0f, 0.0f, 0.0f)
#define useBVH true
#define USE_MAXDEPTH_NEE
// #define USE_MAXDEPTH_NAIVE
#define OVERRIDE_LOCAL_RENDER_VAL TRUE
const int LIGHT_SAMPLE_NUM = LIGHT_SAMPLE_X * LIGHT_SAMPLE_Y;