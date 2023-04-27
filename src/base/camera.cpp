#include "./camera.hpp"

gl::mat4 Camera::getViewMat() const
{
    return gl::getViewMat(position, position + getFront(), getUp());
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float focalDist)
    : fov(fov), aspect(aspect), focalDist(focalDist) {}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float focalDist, gl::vec3 up, gl::vec3 front, gl::vec3 position) : fov(fov), aspect(aspect), focalDist(focalDist)
{
    this->defaultUp = up;
    this->defaultFront = front;
    this->position = position;
}

PerspectiveCamera::PerspectiveCamera(const CameraIO* _io,float aspect) :aspect(aspect) {
    this->defaultUp = _io->orthoUp;
    this->defaultFront = _io->viewDirection;
    this->position = _io->position;
    this->fov = _io->verticalFOV;
    this->focalDist = _io->focalDistance;
}

