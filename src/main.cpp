#include "PBRender.h"
#include "geometry.h"
#include "transform.h"
#include "shapes/sphere.h"
#include "shapes/triangle.h"

#include <stb_image_write.h>

using namespace PBRender;

void test_rainbow() {
    std::vector<char> buf(512 * 512 * 3);
    for (int j = 0; j < 512; j++) {
        for (int i = 0; i < 512; i++) {
            buf[(j * 512 + i) * 3 + 0] = i / 2;
            buf[(j * 512 + i) * 3 + 1] = j / 2;
            buf[(j * 512 + i) * 3 + 2] = 0;
        }
    }
    stbi_write_png("rainbow.png", 512, 512, 3, buf.data(), 0);
}

void test_sphere() {
    Transform sphereT_Object2World, sphereT_World2Object;
    Shape* s = new Sphere(&sphereT_Object2World , &sphereT_World2Object , false , 1.0);

    Vector3f lower_left_corner(-2.0, -2.0, -2.0);
    Vector3f horizontal(4.0, 0.0, 0.0);
    Vector3f vertical(0.0, 4.0, 0.0);

    Point3f origin(0.0, 0.0, -3.0);

    std::vector<char> buf(512 * 512 * 3);

    for (int j = 0; j < 512; j++) {
        for (int i = 0; i < 512; i++) {
            float u = float(i + 0.5) / float(512);
            float v = float(j + 0.5) / float(512);

            Vector3f dir(lower_left_corner + u*horizontal + v*vertical);
            dir -= Vector3f(origin);
            Ray r(origin, dir);
            
            buf[(j * 512 + i) * 3 + 0] = 255;
            buf[(j * 512 + i) * 3 + 1] = 255;
            buf[(j * 512 + i) * 3 + 2] = 0;

            float tHit;
            SurfaceInteraction *isect = nullptr;
            if (s->Intersect(r, &tHit, isect)) {
                buf[(j * 512 + i) * 3 + 0] = 255;
                buf[(j * 512 + i) * 3 + 1] = 0;
                buf[(j * 512 + i) * 3 + 2] = 0;
            }
        }
    }
    stbi_write_png("test_sphere.png", 512, 512, 3, buf.data(), 0);

    delete s;
}

void test_triangle() {
    Transform tri_Object2World , tri_World2Object;

    int nTriangles = 2;
    int vertexIndices [ 6 ] = { 0 ,1 ,2 ,3 ,4 ,5 };
    int nVertices = 6;

    Point3f P[ 6 ] = {
        Point3f ( -1.0 ,1.0 , 0.0) , Point3f ( -1.0 , -1.0 ,0.0) , Point3f ( 0.0 , 1.0 , 0.0 ) ,
        Point3f ( 1.0 , 1.0 , 0.0) , Point3f (1.0 , -1.0 ,0.0) , Point3f (0.0 , -1.0 ,0.0)
    };

    std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(
        tri_Object2World, nTriangles, vertexIndices, nVertices, P, nullptr, nullptr, nullptr, nullptr
    );

    std::vector<std::shared_ptr<Shape>> tris;
    tris.reserve(nTriangles);
    for (size_t i = 0; i < nTriangles; ++i)
    {
        tris.push_back(std::make_shared<Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));
    }

    Vector3f lower_left_corner(-2.0, -2.0, -2.0);
    Vector3f horizontal(4.0, 0.0, 0.0);
    Vector3f vertical(0.0, 4.0, 0.0);

    Point3f origin(0.0, 0.0, -3.0);

    std::vector<char> buf(512 * 512 * 3);

    for (int j = 0; j < 512; j++) {
        for (int i = 0; i < 512; i++) {
            float u = float(i + 0.5) / float(512);
            float v = float(j + 0.5) / float(512);

            Vector3f dir(lower_left_corner + u*horizontal + v*vertical);
            dir -= Vector3f(origin);
            Ray r(origin, dir);
            
            buf[(j * 512 + i) * 3 + 0] = 255;
            buf[(j * 512 + i) * 3 + 1] = 255;
            buf[(j * 512 + i) * 3 + 2] = 0;

            float tHit;
            SurfaceInteraction *isect = nullptr;
            if (tris[0]->Intersect(r, &tHit , isect) ||
                tris[1]->Intersect(r, &tHit , isect)) {
                buf[(j * 512 + i) * 3 + 0] = 255;
                buf[(j * 512 + i) * 3 + 1] = 0;
                buf[(j * 512 + i) * 3 + 2] = 0;
            }
        }
    }
    stbi_write_png("test_triangle.png", 512, 512, 3, buf.data(), 0);
    
}


void renderFrame() {

    Point3f eye( 0.0 , 0.0 , 3.0 ), look( 0.0 , 0.0 , 1.0 ) ;
    Vector3f up( 0.0 , 1.0 , 0.0 ) ;

    Transform lookat = LookAt ( eye , look , up ) ;
    // test_rainbow();
    test_sphere();
    test_triangle();
}

int main(int argc, char *argv[]) {
    renderFrame();
}