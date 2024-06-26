#include <PBRender/PBRender.h>

#include <limits>
#include <iostream>
#include <ctime>

using Point2f = PBRender::Point2f;
using Point3f = PBRender::Point3f;
using Vector3i= PBRender::Vector3i;
using Vector3f= PBRender::Vector3f;

double getSeconds() {
    std::time_t t = std::time(nullptr);
    return static_cast<double>(t);
}

// initialize cornel box scene
void InitCornellBox(PBRender::Scene *scene, std::vector<Vector3f> &albedos) {
    std::vector<Point3f> vertices;
    std::vector<Vector3i> indices;
    size_t geomID;

    // floor
    vertices.clear(); indices.clear();

    vertices.emplace_back(-1, 0,-1);
    vertices.emplace_back(-1, 0, 1);
    vertices.emplace_back( 1, 0, 1);
    vertices.emplace_back( 1, 0,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);
    
    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(0.725f, 0.71f, 0.68f);

    // ceiling
    vertices.clear(); indices.clear();

    vertices.emplace_back( 1, 2, 1);
    vertices.emplace_back(-1, 2, 1);
    vertices.emplace_back(-1, 2,-1);
    vertices.emplace_back( 1, 2,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(0.725f, 0.71f, 0.68f);

    // back wall
    vertices.clear(); indices.clear();

    vertices.emplace_back(-1, 0,-1);
    vertices.emplace_back(-1, 2,-1);
    vertices.emplace_back( 1, 2,-1);
    vertices.emplace_back( 1, 0,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(0.725f, 0.71f, 0.68f);

    // right wall
    vertices.clear(); indices.clear();

    vertices.emplace_back( 1, 0,-1);
    vertices.emplace_back( 1, 2,-1);
    vertices.emplace_back( 1, 2, 1);
    vertices.emplace_back( 1, 0, 1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(0.14f, 0.45f, 0.091f);

    // left wall
    vertices.clear(); indices.clear();

    vertices.emplace_back(-1, 0, 1);
    vertices.emplace_back(-1, 2, 1);
    vertices.emplace_back(-1, 2,-1);
    vertices.emplace_back(-1, 0,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(0.63f, 0.065f, 0.05f);

    // light
    vertices.clear(); indices.clear();

    vertices.emplace_back(-0.24f, 1.98f, -0.22f);
    vertices.emplace_back( 0.23f, 1.98f, -0.22f);
    vertices.emplace_back( 0.23f, 1.98f,  0.16f);
    vertices.emplace_back(-0.24f, 1.98f,  0.16f);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(17.f, 12.f, 4.f) / 255.f;

    // short box
    vertices.clear(); indices.clear();

    vertices.emplace_back(-0.0460751, 0.6, 0.573007);
    vertices.emplace_back(-0.0460751, -2.98023e-8, 0.573007);
    vertices.emplace_back(0.124253, 0, 0.00310463);
    vertices.emplace_back(0.124253, 0.6, 0.00310463);
    vertices.emplace_back(0.533009, 0, 0.746079);
    vertices.emplace_back(0.533009, 0.6, 0.746079);
    vertices.emplace_back(0.703337, 0.6, 0.176177);
    vertices.emplace_back(0.703337, 2.98023e-8, 0.176177);
    vertices.emplace_back(0.533009, 0.6, 0.746079);
    vertices.emplace_back(-0.0460751, 0.6, 0.573007);
    vertices.emplace_back(0.124253, 0.6, 0.00310463);
    vertices.emplace_back(0.703337, 0.6, 0.176177);
    vertices.emplace_back(0.703337, 2.98023e-8, 0.176177);
    vertices.emplace_back(0.124253, 0, 0.00310463);
    vertices.emplace_back(-0.0460751, -2.98023e-8, 0.573007);
    vertices.emplace_back(0.533009, 0, 0.746079);
    vertices.emplace_back(0.533009, 0, 0.746079);
    vertices.emplace_back(-0.0460751, -2.98023e-8, 0.573007);
    vertices.emplace_back(-0.0460751, 0.6, 0.573007);
    vertices.emplace_back(0.533009, 0.6, 0.746079);
    vertices.emplace_back(0.703337, 0.6, 0.176177);
    vertices.emplace_back(0.124253, 0.6, 0.00310463);
    vertices.emplace_back(0.124253, 0, 0.00310463);
    vertices.emplace_back(0.703337, 2.98023e-8, 0.176177);

    indices.emplace_back(0, 2, 1);
    indices.emplace_back(0, 3, 2);
    indices.emplace_back(4, 6, 5);
    indices.emplace_back(4, 7, 6);
    indices.emplace_back(8, 10, 9);
    indices.emplace_back(8, 11, 10);
    indices.emplace_back(12, 14, 13);
    indices.emplace_back(12, 15, 14);
    indices.emplace_back(16, 18, 17);
    indices.emplace_back(16, 19, 18);
    indices.emplace_back(20, 22, 21);
    indices.emplace_back(20, 23, 22);

    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(0.725f, 0.71f, 0.68f);

    // tall box
    vertices.clear(); indices.clear();

    vertices.emplace_back(-0.720444, 1.2, -0.473882);
    vertices.emplace_back(-0.720444, 0, -0.473882);
    vertices.emplace_back(-0.146892, 0, -0.673479);
    vertices.emplace_back(-0.146892, 1.2, -0.673479);
    vertices.emplace_back(-0.523986, 0, 0.0906493);
    vertices.emplace_back(-0.523986, 1.2, 0.0906492);
    vertices.emplace_back(0.0495656, 1.2, -0.108948);
    vertices.emplace_back(0.0495656, 0, -0.108948);
    vertices.emplace_back(-0.523986, 1.2, 0.0906492);
    vertices.emplace_back(-0.720444, 1.2, -0.473882);
    vertices.emplace_back(-0.146892, 1.2, -0.673479);
    vertices.emplace_back(0.0495656, 1.2, -0.108948);
    vertices.emplace_back(0.0495656, 0, -0.108948);
    vertices.emplace_back(-0.146892, 0, -0.673479);
    vertices.emplace_back(-0.720444, 0, -0.473882);
    vertices.emplace_back(-0.523986, 0, 0.0906493);
    vertices.emplace_back(-0.523986, 0, 0.0906493);
    vertices.emplace_back(-0.720444, 0, -0.473882);
    vertices.emplace_back(-0.720444, 1.2, -0.473882);
    vertices.emplace_back(-0.523986, 1.2, 0.0906492);
    vertices.emplace_back(0.0495656, 1.2, -0.108948);
    vertices.emplace_back(-0.146892, 1.2, -0.673479);
    vertices.emplace_back(-0.146892, 0, -0.673479);
    vertices.emplace_back(0.0495656, 0, -0.108948);

    indices.emplace_back(0, 2, 1);
    indices.emplace_back(0, 3, 2);
    indices.emplace_back(4, 6, 5);
    indices.emplace_back(4, 7, 6);
    indices.emplace_back(8, 10, 9);
    indices.emplace_back(8, 11, 10);
    indices.emplace_back(12, 14, 13);
    indices.emplace_back(12, 15, 14);
    indices.emplace_back(16, 18, 17);
    indices.emplace_back(16, 19, 18);
    indices.emplace_back(20, 22, 21);
    indices.emplace_back(20, 23, 22);

    geomID = scene->AddTriMesh(vertices, indices);
    albedos[geomID] = Vector3f(0.725f, 0.71f, 0.68f);
}


int main() {
    std::cout << "Hello Embree!" << std::endl;

    // create engine instance
    auto engine = PBRender::InitEngine();
    std::vector<Vector3f> albedos(10, Vector3f());

    auto scene  = engine->GetScene();
    InitCornellBox(scene.get(), albedos);

    // finish scene
    scene->FinishScene();

    // create a film
    const size_t W = 1024;
    const size_t H = 1024;
    
    PBRender::Vector2f fullResolution(static_cast<float>(W), static_cast<float>(H));

    std::vector<Vector3f> film_normal(H*W, Vector3f(0.f,0.f,0.f));
    std::vector<Vector3f> film_albedo(H*W, Vector3f(0.f,0.f,0.f));
    std::vector<Vector3f> film_depth(H*W, Vector3f(0.f,0.f,0.f));

    // set up camera
    Point3f pos(0.f, 1.f, 6.8f);
    Point3f look(0.f, 1.f, 0.f);
    Vector3f up(0.f, 1.f, 0.f);

    PBRender::Transform world2camera = PBRender::LookAt(pos, look, up);
    PBRender::Transform camera2world = world2camera.Inverse();

    float fov = 19.5f;
    auto camera = PBRender::CreatePerspectiveCamera(camera2world, fullResolution, fov);
    // auto camera = PBRender::CreateOrthographicCamera(camera2world, fullResolution);

    float dmin = PBRender::Infinity;
    float dmax = 0.f;

    // rasterization
    for (size_t i = 0; i < static_cast<size_t>(fullResolution.x); ++i)
    {   
        for (size_t j = 0; j < static_cast<size_t>(fullResolution.y); ++j)
        {
            size_t offset = i + static_cast<size_t>(fullResolution.x) * j;

            // generate a ray from camera
            PBRender::CameraSample sample;
            sample.pFilm = Point2f(i, j);
            PBRender::Ray ray;
            camera->GenerateRay(sample, ray);

            // initialize a rayhit
            RTCRayHit rayhit;
            rayhit.ray.org_x = ray.org.x;
            rayhit.ray.org_y = ray.org.y;
            rayhit.ray.org_z = ray.org.z;
            rayhit.ray.dir_x = ray.dir.x;
            rayhit.ray.dir_y = ray.dir.y;
            rayhit.ray.dir_z = ray.dir.z;
            rayhit.ray.tnear = 0;
            rayhit.ray.tfar = PBRender::Infinity;
            rayhit.ray.mask = -1;
            rayhit.ray.flags = 0;
            rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

            // cast the ray to scene
            scene->RayHit(&rayhit);
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                // shading with normal
                float nx = rayhit.hit.Ng_x * 0.5f + 0.5f;
                float ny = rayhit.hit.Ng_y * 0.5f + 0.5f;
                float nz = rayhit.hit.Ng_z * 0.5f + 0.5f;

                film_normal[offset] = Vector3f(nx,ny,nz);

                // shading with depth
                float depth = rayhit.ray.tfar;
                dmax = std::max(dmax, depth);
                dmin = std::min(dmin, depth);

                film_depth[offset] = Vector3f(depth, depth, depth);

                // shading with albedo
                film_albedo[offset] = albedos[rayhit.hit.geomID];
            }
        }
    }

    std::cout << "Minimum Depth: " << dmin << ", ";
    std::cout << "Maximum Depth: " << dmax << std::endl;

    // write to image
    auto buf_normal = std::vector<char>(3 * film_normal.size());
    auto buf_depth  = std::vector<char>(3 * film_depth.size());
    auto buf_albedo = std::vector<char>(3 * film_albedo.size());

    for (size_t i = 0; i < film_normal.size(); ++i)
    {
        buf_normal[3 * i + 0] = (uint8_t) PBRender::Clamp(255.f * film_normal[i].x, 0.f, 255.f);
        buf_normal[3 * i + 1] = (uint8_t) PBRender::Clamp(255.f * film_normal[i].y, 0.f, 255.f);
        buf_normal[3 * i + 2] = (uint8_t) PBRender::Clamp(255.f * film_normal[i].z, 0.f, 255.f);

        float t = (film_depth[i].x - dmin) / dmax;
        buf_depth[3 * i + 0] = (uint8_t) (t * 255.f);
        buf_depth[3 * i + 1] = (uint8_t) (t * 255.f);
        buf_depth[3 * i + 2] = (uint8_t) (t * 255.f);

        buf_albedo[3 * i + 0] = (uint8_t) PBRender::Clamp(255.f * film_albedo[i].x, 0.f, 255.f);
        buf_albedo[3 * i + 1] = (uint8_t) PBRender::Clamp(255.f * film_albedo[i].y, 0.f, 255.f);
        buf_albedo[3 * i + 2] = (uint8_t) PBRender::Clamp(255.f * film_albedo[i].z, 0.f, 255.f);
    }

    stbi_write_png("output_normal.png", W, H, 3, buf_normal.data(), 0);
    stbi_write_png("output_depth.png", W, H, 3, buf_depth.data(), 0);
    stbi_write_png("output_albedo.png", W, H, 3, buf_albedo.data(), 0);
}