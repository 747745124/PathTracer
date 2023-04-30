#pragma once
#include "../utils/matrix.hpp"
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

    void writeToFile(const std::string &file_path) const;
private:
    uint width;
    uint height;
    uint channels;
    std::vector<std::vector<gl::vec3>> _pixel_color;
};
