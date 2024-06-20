#include "scene.h"

namespace PBRender
{

Scene::Scene(const RTCDevice _device): device(_device) {
    // initialize embree RTCScene
    scene = rtcNewScene(device);
    rtcSetSceneFlags(scene, RTC_SCENE_FLAG_ROBUST);
    rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_HIGH);

    std::cout << "initialize Scene finished!" << std::endl;
}

Scene::~Scene() {
    std::cout << "release Scene..." << std::endl;

    rtcReleaseScene(scene);

    std::cout << "Scene released!" << std::endl;
}

uint Scene::AddTriMesh(const std::vector<Point3f> &vertices, 
                        const std::vector<Vector3i> &indices) {

    std::cout << "Adding a triangle mesh to the Scene..." << std::endl;
    
    // create a triangle mesh geometry and initialize a singe triangle
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    rtcSetGeometryBuildQuality(geom, RTC_BUILD_QUALITY_HIGH);

    size_t n_vertices = vertices.size();
    size_t n_indices  = indices.size();

    std::cout << "n_vertices=" << n_vertices;
    std::cout << ", n_indices=" << n_indices << std::endl << std::endl;

    // vertex buffer
    float* _vertices = (float*) rtcSetNewGeometryBuffer(geom,
                                                        RTC_BUFFER_TYPE_VERTEX,
                                                        0,
                                                        RTC_FORMAT_FLOAT3,
                                                        3*sizeof(float),
                                                        n_vertices);

    // index buffer
    unsigned* _indices = (unsigned*) rtcSetNewGeometryBuffer(geom,
                                                             RTC_BUFFER_TYPE_INDEX,
                                                             0,
                                                             RTC_FORMAT_UINT3,
                                                             3*sizeof(unsigned),
                                                             n_indices);
    
    if (_vertices && _indices) {
        for (size_t i=0; i< n_vertices; ++i)
        {
            _vertices[i * 3]     = vertices[i].x;
            _vertices[i * 3 + 1] = vertices[i].y;
            _vertices[i * 3 + 2] = vertices[i].z;
        }

        for (size_t i=0; i< n_indices; ++i)
        {
            _indices[i * 3]     = indices[i].x;
            _indices[i * 3 + 1] = indices[i].y;
            _indices[i * 3 + 2] = indices[i].z;
        }
    }

    // commit geometry
    rtcCommitGeometry(geom);

    // attach and release geometry 
    uint geomID = rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);

    return geomID;
}

uint Scene::AddSphere(const Point3f center, const float radius) {
    
    std::cout << "Adding a sphere to the Scene..." << std::endl;
    
    // create a triangle mesh geometry and initialize a singe triangle
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_SPHERE_POINT);
    rtcSetGeometryBuildQuality(geom, RTC_BUILD_QUALITY_HIGH);

    // vertex buffer
    float* _vertices = (float*) rtcSetNewGeometryBuffer(geom,
                                                        RTC_BUFFER_TYPE_VERTEX,
                                                        0,
                                                        RTC_FORMAT_FLOAT4,
                                                        4*sizeof(float),
                                                        1);
    
    _vertices[0] = center.x;
    _vertices[1] = center.y;
    _vertices[2] = center.z;
    _vertices[3] = radius;

    // commit geometry
    rtcCommitGeometry(geom);

    // attach and release geometry 
    uint geomID = rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);

    return geomID;
}

void Scene::RayHit(RTCRayHit *rayhit) {
    rtcIntersect1(scene, rayhit);
}

} // namespace PBRender