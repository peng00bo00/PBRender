#include "PBRender.h"

#include "geometry.h"
#include "transform.h"
#include "modelloader.h"

#include "accelerators/bvh.h"
#include "cameras/orthographic.h"
#include "cameras/perspective.h"
#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "spectrum.h"

#include <stb_image_write.h>


using namespace PBRender;

std::vector<char> color2Img(std::vector<Spectrum> col) {
    int N = col.size();
    std::vector<char> buf(N * 3);

    for (size_t i = 0; i < N; ++i)
    {
        buf[3 * i + 0] = 255 * col[i][0];
        buf[3 * i + 1] = 255 * col[i][1];
        buf[3 * i + 2] = 255 * col[i][2];
    }
    
    return buf;
}

void test() {

    Transform Object2WorldModel = Scale( 0.2, 0.2, 0.2 );
    Object2WorldModel = Translate(Vector3f(0.0, 0.0, 0.0)) * Object2WorldModel;
    std::vector<std::shared_ptr<Primitive>> prims;

    ModelLoader loader;
    loader.loadModel("./teapot.obj", Object2WorldModel);
    loader.buildNoTextureModel(Object2WorldModel, prims);

    auto agg = CreateBVHAccelerator(prims);

    // initialize camera
    Camera *cam;
    Point3f eye( 0.0f, 0.0f , -3.0f), look( 0.0, 0.0, 0.0f );
    Vector3f up(0.0f, 1.0f, 0.0f);

    Transform lookat = LookAt ( eye, look, up ) ;
    Transform Camera2World = Inverse(lookat);

    Vector2f fullResolution(500, 300);
    // cam = CreateOrthographicCamera( Camera2World, fullResolution ) ;
    cam = CreatePerspectiveCamera( Camera2World, fullResolution );

    Vector3f Light(1.0, 1.0, 1.0);
    Light = Normalize(Light);

    std::vector<Spectrum> col(int(fullResolution.x) * int(fullResolution.y));
    std::cout << "Rendering begins!" << std::endl;

    for (size_t i = 0; i < int(fullResolution.x); i++)
    {
        for (size_t j = 0; j < int(fullResolution.y); j++)
        {
            CameraSample cs;
            cs.pFilm = Point2f(i+0.5f, j+0.5f);

            Ray r;
            cam->GenerateRay( cs, &r );

            SurfaceInteraction isect;

            Spectrum colObj(0.0f);
            if (agg->Intersect(r, &isect)) {
                float Li = Dot(Light , isect.n );
                colObj[0] = 0;
                colObj[1] = std::abs(Li);
                colObj[2] = 0;
            }

            col[i + j * int(fullResolution.x)] = colObj;
        }
        
    }

    auto buf = color2Img(col);
    std::cout << "Rendering is finished!" << std::endl;
    stbi_write_png("test.png", int(fullResolution.x), int(fullResolution.y), 3, buf.data(), 0);

    delete cam;
}

int main(int argc, char *argv[]) {

    test();
    std::cout << "Finish!" << std::endl;

    return 0;
}