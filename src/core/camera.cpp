#include <PBRender/core/camera.h>

namespace PBRender
{
Camera::~Camera() {}

Camera::Camera(const Transform &CameraToWorld)
    : CameraToWorld(CameraToWorld) {}

} // namespace PBRender