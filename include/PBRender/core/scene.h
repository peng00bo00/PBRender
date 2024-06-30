#pragma once

#include <embree4/rtcore.h>

#include <PBRender/core/common.h>
#include <PBRender/core/math.h>
#include <PBRender/core/vecmath.h>


namespace PBRender
{

// vertex attributes
enum VERTEX_ATTRIB_SLOT {
    VERTEX_ALBEDO = 0,

    // number of vertex attributes
    NUM_VERTX_ATTRIB = 1
};


class Scene {
public:
    Scene(const RTCDevice _device);
    ~Scene();

    uint AddTriMesh(const std::vector<Point3f> &vertices, const std::vector<Vector3i> &indices);
    uint AddTriMesh(const std::vector<Point3f> &vertices, const std::vector<Vector3i> &indices, const Vector3f albedo);

    uint AddSphere(const Point3f center, const float radius);

    RTCGeometry GetGeometry(uint geomID);

    void FinishScene() { rtcCommitScene(scene); };

    void RayHit(RTCRayHit *rayhit);

private:
    const RTCDevice device;
    RTCScene scene;
};

} // namespace PBRender