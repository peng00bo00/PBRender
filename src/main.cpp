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

#include <random>
#include <omp.h>


using namespace PBRender;

constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;

int NUM_PROCS =  omp_get_num_procs();

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

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

    Transform Object2WorldModel = Scale( 1.0, 1.0, 1.0 );
    Object2WorldModel = Translate(Vector3f(0.0, -1.0, 3.0)) * Object2WorldModel;
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

    Vector2f fullResolution(800, 600);
    // cam = CreateOrthographicCamera( Camera2World, fullResolution ) ;
    cam = CreatePerspectiveCamera( Camera2World, fullResolution, 90.0f);
    // cam = CreatePerspectiveCamera( Camera2World, fullResolution, 90.0f, 0.07f, 1.0f );

    Vector3f Light(10.0, 10.0,-1.0);
    Light = Normalize(Light);

    std::vector<Spectrum> col(int(fullResolution.x) * int(fullResolution.y));
    std::cout << "Rendering begins! " << "Using " << NUM_PROCS << " cores." << std::endl;

    omp_set_num_threads(NUM_PROCS);
    #pragma omp parallel for
    for (size_t i = 0; i < int(fullResolution.x); i++)
    {
        for (size_t j = 0; j < int(fullResolution.y); j++)
        {
            CameraSample cs;
            cs.pFilm = Point2f(i+0.5f, j+0.5f);
            float random = distr(eng);
            cs.pLens = Point2f ( random, random );
            Ray r;
            cam->GenerateRay( cs, &r );

            SurfaceInteraction isect;

            Spectrum colObj(0.0f);
            if (agg->Intersect(r, &isect)) {
                float Li = Dot(Light , isect.n );
                // Li = std::abs(Li);
                Li = Clamp(Li, 0.01f, 1.0f);
                Li = sqrt(Li);

                colObj[0] = Li;
                colObj[1] = Li;
                colObj[2] = Li;
            }

            col[i + j * int(fullResolution.x)] = colObj;
        }
        
    }

    auto buf = color2Img(col);
    std::cout << "Rendering is finished!" << std::endl;

    // stbi_write_png("OrthographicCamera.png", int(fullResolution.x), int(fullResolution.y), 3, buf.data(), 0);
    stbi_write_png("PerspectiveCamera.png", int(fullResolution.x), int(fullResolution.y), 3, buf.data(), 0);

    delete cam;
}

int main(int argc, char *argv[]) {

    test();
    std::cout << "Finish!" << std::endl;

    return 0;
}