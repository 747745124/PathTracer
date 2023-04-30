#include "./framebuffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"

FrameBuffer::FrameBuffer(uint width, uint height, uint channels)
{   
    //row major
    this->width = width;
    this->height = height;
    this->channels = channels;
    _pixel_color.resize(height);
    for (uint i = 0; i < height; i++)
    {
        _pixel_color[i].resize(width);
    }
}

void FrameBuffer::setPixelColor(uint u, uint v, gl::vec3 color)
{
    this->_pixel_color[u][v] = color;
}

void FrameBuffer::writeToFile(const std::string &file_path) const
{
    std::vector<uint8_t> data;
    data.reserve(this->width * this->height * this->channels);
    for (uint i = 0; i < this->height; i++)
    {
        for (uint j = 0; j < this->width; j++)
        {
            for (uint k = 0; k < this->channels; k++)
            {
                data.push_back(static_cast<uint8_t>(this->_pixel_color[i][j][k] * 255));
            }
        }
    }
    
    stbi_flip_vertically_on_write(1);
    stbi_write_png(file_path.c_str(), this->width, this->height, this->channels, data.data(), this->width * this->channels);
};
