#pragma once

#include <embree4/rtcore.h>

#include <PBRender/core/common.h>
#include <PBRender/core/material.h>
#include <PBRender/core/math.h>
#include <PBRender/core/vecmath.h>


namespace PBRender
{

// vertex attributes
enum VERTEX_ATTRIB_SLOT : unsigned int {
    TEXTURE_COORD = 0U,
    VERTEX_ALBEDO = 1U,

    // number of vertex attributes
    NUM_VERTX_ATTRIB = 2U
};

struct GeometryEntity {
    uint geomID;
    uint materialID;
};


class Scene {
public:
    Scene(const RTCDevice _device);
    ~Scene();

    uint AddTriMesh(const std::vector<Point3f> &vertices, 
                    const std::vector<Vector3i> &indices, 
                    const std::vector<Point2f> &tex_coord, 
                    const Vector3f albedo);
    uint AddTriMesh(const std::vector<Point3f> &vertices, 
                    const std::vector<Vector3i> &indices,
                    const Vector3f albedo);

    uint AddSphere(const Point3f center, const float radius);

    RTCGeometry GetGeometry(uint geomID);

    void FinishScene() { rtcCommitScene(scene); };

    void RayHit(RTCRayHit *rayhit);

public:
    std::vector<GeometryEntity> geometries;
    std::vector<Material> materials;

private:
    const RTCDevice device;
    RTCScene scene;
};

} // namespace PBRender