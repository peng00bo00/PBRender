#include "PBRender.h"
#include "geometry.h"
#include "transform.h"

using namespace PBRender;

int main(int argc, char *argv[]) {
    auto x = Vector2f(0.0, 1.2);
    auto y = Vector3f(0.0, 1.2, 2.4);
    auto z = Vector3f(-1.0, 2, 3.4);
    std::cout << x*2 << std::endl;
    std::cout << y/2.0 << std::endl;

    auto m = Matrix4x4();
    std::cout << m << std::endl;
}