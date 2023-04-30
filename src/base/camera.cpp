#include "./camera.hpp"

gl::mat4 Camera::getViewMat() const
{
    return gl::getViewMat(position, position + getFront(), getUp());
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float focalDist, gl::vec3 up, gl::vec3 front, gl::vec3 position) : fov(fov), aspect(aspect), focalDist(focalDist)
{
    using namespace gl;
    // assume fov is given in radian
    auto theta = fov / 2.f;
    auto h = tan(theta);
    auto plane_height = h * 2.0f;
    auto plane_width = plane_height * aspect;

    this->defaultUp = up;
    this->defaultFront = front;
    this->position = position;

    auto lookat = -defaultFront.normalize();
    auto plane_h_vec = cross(up, lookat).normalize();
    // std::cout<<plane_h_vec<<std::endl;
    auto plane_v_vec = cross(lookat, plane_h_vec).normalize();
    // std::cout<<plane_v_vec<<std::endl;

    this->plane_horizontal = plane_h_vec * plane_width;
    this->plane_vertical = plane_v_vec * plane_height;
    bottom_left_pos = position - this->plane_horizontal / 2.0f - this->plane_vertical / 2.0f;
    bottom_left_pos -= lookat;
}

PerspectiveCamera::PerspectiveCamera(const CameraIO *_io, float aspect)
    : PerspectiveCamera(_io->verticalFOV, aspect, _io->focalDistance, _io->orthoUp, _io->viewDirection, _io->position)
{
}

Ray PerspectiveCamera::generateRay(float u, float v) const
{
    using namespace gl;
    auto og = this->position;
    auto dir = bottom_left_pos + u * plane_horizontal + v * plane_vertical - og;
    return Ray(og, dir);
}
