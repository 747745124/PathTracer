#pragma once
#include "./utility.hpp"
#include "./matrix.hpp"

namespace gl
{

    // returns the index for gradient
    static std::array<int, 512U> perm = {151, 160, 137, 91, 90, 15,
                                      131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
                                      190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
                                      88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
                                      77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
                                      102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
                                      135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
                                      5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
                                      223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
                                      129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
                                      251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
                                      49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
                                      138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15,
                                      131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
                                      190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
                                      88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
                                      77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
                                      102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
                                      135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
                                      5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
                                      223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
                                      129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
                                      251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
                                      49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
                                      138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

    static int get_next(int num, int size)
    {
        return (num + 1) % size;
    };

    // Source: http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
    static float grad(int hash, float x, float y)
    {
        switch (hash & 0x7)
        {
        case 0x0:
            return x + y;
        case 0x1:
            return -x + y;
        case 0x2:
            return x - y;
        case 0x3:
            return -x - y;
        case 0x4:
            return x;
        case 0x5:
            return -x;
        case 0x6:
            return y;
        case 0x7:
            return -y;
        default:
            return 0;
        }
    }

    static float perlin_noise_2D(gl::vec2 uv, int size = 256)
    {

        float x = fmodf(uv.x(), size);
        float y = fmodf(uv.y(), size);

        //integer part
        int X = (int)floor(x) & 255;
        int Y = (int)floor(y) & 255;

        //extract the fractional part
        x = fract(x);
        y = fract(y);

        float u = smoothstep_alt(0.f, 1.f, x);
        float v = smoothstep_alt(0.f, 1.f, y);

        int aa = perm[perm[X] + Y];
        int ab = perm[perm[X] + get_next(Y, size)];
        int ba = perm[perm[get_next(X, size)] + Y];
        int bb = perm[perm[get_next(X, size)] + get_next(Y, size)];

        float x1 = lerp(grad(aa, x, y), grad(ba, x - 1, y), u);
        float x2 = lerp(grad(ab, x, y - 1), grad(bb, x - 1, y - 1), u);
        float y1 = lerp(x1, x2, v);

        return (y1+1.f)/2.f;
    };

    static float fractal_perlin_2D(gl::vec2 uv, int octaves){
        float total = 0.f;
        float amp = 1.f;
        float freq = 1.f;
        float max_val = 0;

        for(int i = 0; i < octaves; i++){
            total += perlin_noise_2D(uv.u() * freq, uv.v() * freq) * amp;
            max_val += amp;
            amp *= 0.5f;
            freq *= 2.f;
        }

        return total/max_val;
    }

};