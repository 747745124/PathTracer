#pragma once
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../utils/matrix.hpp"
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"
#include <cstdint>
#ifdef _WIN32
#include <Windows.h>
using uint = unsigned int;
#endif

class FrameBuffer
{
public:
    FrameBuffer(uint width, uint height, uint channels = 3);

    void setPixelColor(uint u, uint v, gl::vec3 color);
    // the input should be 2 integers
    gl::vec3 getPixelColor(uint u, uint v) const
    {
        return this->_pixel_color[u][v];
    };

    uint getWidth() const
    {
        return this->width;
    };

    uint getHeight() const
    {
        return this->height;
    };

    void writeToFile(const std::string &file_path) const
    {
        std::vector<uint8_t> data;
        data.reserve(this->width * this->height * this->channels);
        for (uint i = 0; i < this->width; i++)
        {
            for (uint j = 0; j < this->height; j++)
            {
                for (uint k = 0; k < this->channels; k++)
                {
                    data.push_back(static_cast<uint8_t>(this->_pixel_color[i][j][k] * 255));
                }
            }
        }
        stbi_write_png(file_path.c_str(), this->width, this->height, this->channels, data.data(), this->width * this->channels);
    };

private:
    uint width;
    uint height;
    uint channels;
    std::vector<std::vector<gl::vec3>> _pixel_color;
};
