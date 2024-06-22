#include <PBRender/PBRender.h>

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
    auto scene = engine->scene;

    // add a sphere
    Point3f center(Point3f(0.0f, 0.0f, 1.0f));
    float radius = 0.5f;
    scene->AddSphere(center, radius);
    scene->FinishScene();

    // create a film
    const size_t H = 1024;
    const size_t W = 1024;
    
    const float xmin = -1.0f;
    const float xmax =  1.0f;
    const float ymin = -1.0f;
    const float ymax =  1.0f;

    std::vector<Vector3f> film_normal(H*W, Vector3f(0.f,0.f,0.f));
    std::vector<Vector3f> film_depth(H*W, Vector3f(0.f,0.f,0.f));

    // rasterization
    for (size_t i = 0; i < H; ++i)
    {   
        float ty = (i+0.5f) / H;
        float yy = PBRender::Lerp(ty, ymin, ymax);

        for (size_t j = 0; j < W; ++j)
        {
            float tx = (j+0.5f) / W;
            float xx = PBRender::Lerp(tx, xmin, xmax);

            // initialize a ray
            RTCRayHit rayhit;
            rayhit.ray.org_x = xx;
            rayhit.ray.org_y = yy;
            rayhit.ray.org_z = 0.f;
            rayhit.ray.dir_x = 0.f;
            rayhit.ray.dir_y = 0.f;
            rayhit.ray.dir_z = 1.f;
            rayhit.ray.tnear = 0;
            rayhit.ray.tfar = std::numeric_limits<float>::infinity();
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

                film_normal[i*H+j] = Vector3f(nx,ny,nz);

                // shading with depth
                float depth = rayhit.ray.tfar;
                film_depth[i*H+j] = Vector3f(depth, depth, depth);
            }
        }
    }

    // write to image
    auto buf_normal = std::vector<char>();
    auto buf_depth = std::vector<char>();

    buf_normal.resize(3 * film_normal.size());
    buf_depth.resize(3 * film_normal.size());

    for (size_t i = 0; i < film_normal.size(); ++i)
    {
        buf_normal[3 * i + 0] = (uint8_t) PBRender::Clamp(255.f * film_normal[i].x + 0.5f, 0.f, 255.f);
        buf_normal[3 * i + 1] = (uint8_t) PBRender::Clamp(255.f * film_normal[i].y + 0.5f, 0.f, 255.f);
        buf_normal[3 * i + 2] = (uint8_t) PBRender::Clamp(255.f * film_normal[i].z + 0.5f, 0.f, 255.f);
    
        buf_depth[3 * i + 0] = (uint8_t) PBRender::Clamp(255.f * film_depth[i].x + 0.5f, 0.f, 255.f);
        buf_depth[3 * i + 1] = (uint8_t) PBRender::Clamp(255.f * film_depth[i].y + 0.5f, 0.f, 255.f);
        buf_depth[3 * i + 2] = (uint8_t) PBRender::Clamp(255.f * film_depth[i].z + 0.5f, 0.f, 255.f);

    }

    stbi_write_png("output_normal.png", W, H, 3, buf_normal.data(), 0);
    stbi_write_png("output_depth.png", W, H, 3, buf_depth.data(), 0);
    
    
    // geomID = scene->AddTriMesh(vertices, indices);
    // std::cout << "Triangle Mesh ID: " << geomID << std::endl;

    // // for (size_t i=0; i<10; ++i)
    // // {
    // //     const float xx = float(drand48())-0.5;
    // //     const float yy = float(drand48())-0.5;
    // //     const float zz = float(drand48())-0.5;
    // //     const float r  = float(drand48());

    // //     geomID = scene->AddSphere(Point3f(xx, yy, zz), r);
    // //     std::cout << "Sphere ID: " << geomID << std::endl;
    // // }

    // // // scene->AddRandomTriangles();
    // // std::cout << "adding random triangles finished!" << std::endl;

    // std::cout << "commit scene..." << std::endl;
    // scene->FinishScene();
    // std::cout << "commit scene finished!" << std::endl;

    // // double t1 = getSeconds();
    // for (size_t i = 0; i < 100; i++)
    // {
    //     // random origin
    //     const float ox = float(drand48())-0.5;
    //     const float oy = float(drand48())-0.5;
    //     const float oz = float(drand48())-0.5;

    //     // random dir
    //     const float dx = float(drand48())-0.5;
    //     const float dy = float(drand48())-0.5;
    //     const float dz = float(drand48())-0.5;

    //     Vector3f dir(dx, dy, dz);
    //     dir = Normalize(dir);
        
    //     RTCRayHit rayhit;
    //     rayhit.ray.org_x = ox;
    //     rayhit.ray.org_y = oy;
    //     rayhit.ray.org_z = oz;
    //     rayhit.ray.dir_x = dx;
    //     rayhit.ray.dir_y = dy;
    //     rayhit.ray.dir_z = dz;
    //     rayhit.ray.tnear = 0;
    //     rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    //     rayhit.ray.mask = -1;
    //     rayhit.ray.flags = 0;
    //     rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    //     rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    //     scene->RayHit(&rayhit);
    //     if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
    //         printf("Ray %zu finds intersection on geometry %d, primitive %d.\n", 
    //         i,
    //         rayhit.hit.geomID,
    //         rayhit.hit.primID);
    //     } else {
    //         printf("Ray %zu does not find any intersection.\n\n", i);
    //     }
    // }
}