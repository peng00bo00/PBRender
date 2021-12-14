#include "PBRender.h"
#include "geometry.h"
#include "transform.h"
#include "shapes/sphere.h"

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


void renderFrame() {

    Point3f eye( 0.0 , 0.0 , 3.0 ), look( 0.0 , 0.0 , 1.0 ) ;
    Vector3f up( 0.0 , 1.0 , 0.0 ) ;

    Transform lookat = LookAt ( eye , look , up ) ;
    // test_rainbow();
    test_sphere();
}

int main(int argc, char *argv[]) {
    // auto x = Vector2f(0.0, 1.2);
    // auto y = Vector3f(0.0, 1.2, 2.4);
    // auto z = Vector3f(-1.0, 2, 3.4);
    // std::cout << x*2 << std::endl;
    // std::cout << y/2.0 << std::endl;

    // auto m = Matrix4x4();
    // std::cout << m << std::endl;

    // Transform t1 = Translate(Vector3f(3.3, 2.0, 0));
    // Point3f c1 = t1(Point3f());
    // std::cout << c1 << std::endl;

    renderFrame();
}