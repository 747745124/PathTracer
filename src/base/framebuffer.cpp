#include "./framebuffer.hpp"
FrameBuffer::FrameBuffer(uint width, uint height, uint channels){
    this->width = width;
    this->height = height;
    this->channels = channels;
    _pixel_color.resize(width);
    for (uint i = 0; i < width; i++)
    {
        _pixel_color[i].resize(height);
    }
}

void FrameBuffer::setPixelColor(uint u, uint v, gl::vec3 color){
    this->_pixel_color[u][v] = color;
}