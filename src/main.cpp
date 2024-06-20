#include "PBRender.h"

#include <limits>
#include <iostream>
#include <ctime>

using Point3f = PBRender::Point3f;
using Vector3i= PBRender::Vector3i;
using Vector3f= PBRender::Vector3f;

double getSeconds() {
    std::time_t t = std::time(nullptr);
    return static_cast<double>(t);
}


int main() {
    std::cout << "Hello Embree!" << std::endl;

    // create engine instance
    auto engine = std::make_unique<PBRender::Engine>();

    std::cout << "initialize Scene..." << std::endl;
    // auto scene = std::make_unique<PBRender::Scene>(engine->device);
    auto scene = engine->scene;

    // std::cout << "adding random triangles to the Scene..." << std::endl;
    size_t N = 10000;
    uint geomID;
    
    std::vector<Point3f> vertices;
    std::vector<Vector3i> indices;

    vertices.reserve(N);
    indices.reserve(N);

    for (size_t i=0; i<N; ++i)
    {
        const float xx = float(drand48())-0.5;
        const float yy = float(drand48())-0.5;
        const float zz = float(drand48())-0.5;

        vertices.emplace_back(xx*100.f, yy*100.f, zz*100.f);
        indices.emplace_back(i*3, i * 3 + 1,i * 3 + 2);
    }

    geomID = scene->AddTriMesh(vertices, indices);
    std::cout << "Triangle Mesh ID: " << geomID << std::endl;

    // for (size_t i=0; i<10; ++i)
    // {
    //     const float xx = float(drand48())-0.5;
    //     const float yy = float(drand48())-0.5;
    //     const float zz = float(drand48())-0.5;
    //     const float r  = float(drand48());

    //     geomID = scene->AddSphere(Point3f(xx, yy, zz), r);
    //     std::cout << "Sphere ID: " << geomID << std::endl;
    // }

    // // scene->AddRandomTriangles();
    // std::cout << "adding random triangles finished!" << std::endl;

    std::cout << "commit scene..." << std::endl;
    scene->FinishScene();
    std::cout << "commit scene finished!" << std::endl;

    // double t1 = getSeconds();

    // // castRay(*scene, 1.00f, 1.00f, -1, 0, 0, 1);
    for (size_t i = 0; i < 100; i++)
    {
        // random origin
        const float ox = float(drand48())-0.5;
        const float oy = float(drand48())-0.5;
        const float oz = float(drand48())-0.5;

        // random dir
        const float dx = float(drand48())-0.5;
        const float dy = float(drand48())-0.5;
        const float dz = float(drand48())-0.5;

        Vector3f dir(dx, dy, dz);
        dir = Normalize(dir);
        
        RTCRayHit rayhit;
        rayhit.ray.org_x = ox;
        rayhit.ray.org_y = oy;
        rayhit.ray.org_z = oz;
        rayhit.ray.dir_x = dx;
        rayhit.ray.dir_y = dy;
        rayhit.ray.dir_z = dz;
        rayhit.ray.tnear = 0;
        rayhit.ray.tfar = std::numeric_limits<float>::infinity();
        rayhit.ray.mask = -1;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

        scene->RayHit(&rayhit);
        if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            printf("Ray %zu finds intersection on geometry %d, primitive %d.\n", 
            i,
            rayhit.hit.geomID,
            rayhit.hit.primID);
        } else {
            printf("Ray %zu does not find any intersection.\n\n", i);
        }
    }
}