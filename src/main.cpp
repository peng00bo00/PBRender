#include "PBRender.h"

#include "geometry.h"
#include "transform.h"
#include "modelloader.h"

#include "accelerators/bvh.h"
#include "cameras/orthographic.h"
#include "cameras/perspective.h"

#include "integrator.h"

#include "scene.h"

#include "sampler.h"
#include "samplers/halton.h"

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "spectrum.h"

#include <stb_image_write.h>

#include <omp.h>


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

    // initialize worldScene
    Transform Object2WorldModel = Scale( 1.0, 1.0, 1.0 );
    Object2WorldModel = Translate(Vector3f(0.0, 0.0, 1.0)) * Object2WorldModel;
    std::vector<std::shared_ptr<Primitive>> prims;

    ModelLoader loader;
    loader.loadModel("./teapot.obj", Object2WorldModel);
    loader.buildNoTextureModel(Object2WorldModel, prims);

    auto agg = CreateBVHAccelerator(prims);

    std::unique_ptr<Scene> worldScene;
    worldScene = std::make_unique<Scene>(agg);

    // initialize camera
    Point3f eye( 0.0f, 5.0f , -3.0f), look( 0.0, 0.0, 1.0f );
    Vector3f up(0.0f, 1.0f, 0.0f);

    Transform lookat = LookAt ( eye, look, up ) ;
    Transform Camera2World = Inverse(lookat);

    Vector2f fullResolution(800, 600);
    std::shared_ptr<Camera> camera;
    camera = std::shared_ptr<PerspectiveCamera>(CreatePerspectiveCamera(Camera2World, fullResolution, 90.0f));

    // sampler
    Bounds2i imageBound(Point2i(0, 0), Point2i(fullResolution.x, fullResolution.y));
    std::shared_ptr<Sampler> sampler = std::shared_ptr<HaltonSampler>(CreateHaltonSampler(32, imageBound));

    std::vector<Spectrum> col(int(fullResolution.x) * int(fullResolution.y));

    // integrator
    std::shared_ptr<Integrator> integrator;
    integrator = std::make_shared<SamplerIntegrator>(camera, sampler, imageBound);
    integrator->Test(*worldScene, fullResolution, col);

    auto buf = color2Img(col);

    stbi_write_png("test.png", int(fullResolution.x), int(fullResolution.y), 3, buf.data(), 0);
}

int main(int argc, char *argv[]) {

    test();
    std::cout << "Finish!" << std::endl;

    return 0;
}