#pragma once

#include <embree4/rtcore.h>

#include "common.h"
#include "math.h"
#include "vecmath.h"


namespace PBRender
{
class Scene {
public:
    Scene(const RTCDevice _device);
    ~Scene();

    uint AddTriMesh(const std::vector<Point3f> &vertices, const std::vector<Vector3i> &indices);
    uint AddSphere(const Point3f center, const float radius);

    void FinishScene() { rtcCommitScene(scene); };

    void RayHit(RTCRayHit *rayhit);

private:
    const RTCDevice device;
    RTCScene scene;
};

} // namespace PBRender