#pragma once
#include "../utils/matrix.hpp"
#include "../utils/quat.hpp"
#include "../utils/transformations.hpp"
#include "./object3D.hpp"
#include "../utils/scene_io.hpp"
#include "./ray.hpp"

// abstract camera class
class Camera : public Object3D
{
public:
    gl::mat4 getViewMat() const;
};

class PerspectiveCamera : public Camera
{
public:
    float fov;
    float aspect;
    float focalDist;
    PerspectiveCamera(float fov, float aspect, float focalDist, gl::vec3 up, gl::vec3 front, gl::vec3 position);
    PerspectiveCamera(const CameraIO* cameraIO,float aspect=1.0f);
    ~PerspectiveCamera() = default;
    Ray generateRay(float x, float y) const;
private:
    gl::vec3 bottom_left_pos;
    gl::vec3 plane_horizontal;
    gl::vec3 plane_vertical;
};
