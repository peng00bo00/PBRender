#include <PBRender/core/scene.h>

namespace PBRender
{

Scene::Scene(const RTCDevice _device): device(_device) {
    // initialize embree RTCScene
    scene = rtcNewScene(device);
    rtcSetSceneFlags(scene, RTC_SCENE_FLAG_ROBUST);
    rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_HIGH);

    std::cout << "Initialize Scene finished!" << std::endl;
}

Scene::~Scene() {
    rtcReleaseScene(scene);
    std::cout << "Scene released!" << std::endl;
}

uint Scene::AddTriMesh(const std::vector<Point3f> &vertices, 
                        const std::vector<Vector3i> &indices, 
                        const std::vector<Point2f> &tex_coord, 
                        Vector3f albedo) {
    // create a triangle mesh geometry
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    rtcSetGeometryBuildQuality(geom, RTC_BUILD_QUALITY_HIGH);

    size_t n_vertices = vertices.size();
    size_t n_indices  = indices.size();

    std::cout << "n_vertices: " << n_vertices << std::endl;
    std::cout << "n_indices: " << n_indices << std::endl;

    // vertex buffer
    float* _vertices = (float*) rtcSetNewGeometryBuffer(geom,
                                                        RTC_BUFFER_TYPE_VERTEX,
                                                        0,
                                                        RTC_FORMAT_FLOAT3,
                                                        3*sizeof(float),
                                                        n_vertices);

    std::cout << "Creating vertex buffer is complete." << std::endl;

    // index buffer
    unsigned* _indices = (unsigned*) rtcSetNewGeometryBuffer(geom,
                                                             RTC_BUFFER_TYPE_INDEX,
                                                             0,
                                                             RTC_FORMAT_UINT3,
                                                             3*sizeof(unsigned),
                                                             n_indices);

    std::cout << "Creating indices buffer is complete." << std::endl;
    
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

    std::cout << "Finish adding vertex and indices." << std::endl;

    // set number of vertex attributes, only use albedo for now
    rtcSetGeometryVertexAttributeCount(geom, VERTEX_ATTRIB_SLOT::NUM_VERTX_ATTRIB);
    
    // texture coordinate
    float* _tex_coord = (float *) rtcSetNewGeometryBuffer(geom, 
                                                        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 
                                                        VERTEX_ATTRIB_SLOT::TEXTURE_COORD,
                                                        RTC_FORMAT_FLOAT2, 
                                                        2*sizeof(float), 
                                                        n_vertices);

    for (size_t i=0; i< n_vertices; ++i)
    {
        _tex_coord[i * 2]     = tex_coord[i].x;
        _tex_coord[i * 2 + 1] = tex_coord[i].y;
    }

    std::cout << "Finish adding texture coordinate." << std::endl;

    // albedo
    float* _albedo = (float*) rtcSetNewGeometryBuffer(geom, 
                                                      RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 
                                                      VERTEX_ATTRIB_SLOT::VERTEX_ALBEDO,
                                                      RTC_FORMAT_FLOAT3, 
                                                      3*sizeof(float), 
                                                      n_vertices);
    
    for (size_t i=0; i< n_vertices; ++i)
    {
        _albedo[i * 3]     = albedo.x;
        _albedo[i * 3 + 1] = albedo.y;
        _albedo[i * 3 + 2] = albedo.z;
    }

    std::cout << "Finish adding vertex albedo." << std::endl;

    // commit geometry
    rtcCommitGeometry(geom);

    // attach and release geometry 
    uint geomID = rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);

    return geomID;
}

uint Scene::AddTriMesh(const std::vector<Point3f> &vertices, 
                        const std::vector<Vector3i> &indices,
                        const Vector3f albedo) {
    // initialize texture coordinates to (0, 0)
    size_t n_vertices = vertices.size();
    std::cout << "num of vertices: " << n_vertices << std::endl;
    std::vector<Point2f> tex_coord(n_vertices, Point2f());
    std::cout << "num of tex_coord: " << n_vertices << std::endl;

    return AddTriMesh(vertices, indices, tex_coord, albedo);
}

uint Scene::AddSphere(const Point3f center, const float radius) {
    
    // std::cout << "Adding a sphere to the Scene..." << std::endl;
    
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

RTCGeometry Scene::GetGeometry(uint geomID) {
    return rtcGetGeometry(scene, geomID);    
}

void Scene::RayHit(RTCRayHit *rayhit) {
    rtcIntersect1(scene, rayhit);
}

} // namespace PBRender