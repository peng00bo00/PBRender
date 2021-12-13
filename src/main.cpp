#include "PBRender.h"
#include "geometry.h"
#include "transform.h"
#include "shapes/sphere.h"

using namespace PBRender;

void renderFrame() {
    Transform sphereT_Object2World, sphereT_World2Object;
    Shape* s = new Sphere(&sphereT_Object2World , &sphereT_World2Object , false , 1.0);

    delete s;

    Point3f eye( 0.0 , 0.0 , 3.0 ), look( 0.0 , 0.0 , 1.0 ) ;
    Vector3f up( 0.0 , 1.0 , 0.0 ) ;

    Transform lookat = LookAt ( eye , look , up ) ;

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