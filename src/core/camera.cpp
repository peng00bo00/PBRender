#include <PBRender/core/camera.h>

namespace PBRender
{
Camera::~Camera() {}

Camera::Camera(const Transform &CameraToWorld, Film *film)
    : CameraToWorld(CameraToWorld), film(film) {}

} // namespace PBRender