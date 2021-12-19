#include "PBRender.h"

#include "geometry.h"
#include "transform.h"
#include "modelloader.h"

#include "accelerators/bvh.h"
#include "cameras/orthographic.h"
#include "cameras/perspective.h"

#include "scene.h"

#include "sampler.h"
#include "samplers/halton.h"

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "spectrum.h"

#include <stb_image_write.h>

#include <omp.h>


using namespace PBRender;

int NUM_PROCS =  omp_get_num_procs();

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
    Camera *cam;
    Point3f eye( 0.0f, 5.0f , -3.0f), look( 0.0, 0.0, 1.0f );
    Vector3f up(0.0f, 1.0f, 0.0f);

    Transform lookat = LookAt ( eye, look, up ) ;
    Transform Camera2World = Inverse(lookat);

    Vector2f fullResolution(800, 600);
    cam = CreatePerspectiveCamera( Camera2World, fullResolution, 90.0f);

    Vector3f Light(10.0, 10.0,-1.0);
    Light = Normalize(Light);

    // sampler
    Bounds2i imageBound(Point2i(0, 0), Point2i(fullResolution.x, fullResolution.y));
    std::shared_ptr<Sampler> hns = std::shared_ptr<HaltonSampler>(CreateHaltonSampler(32, imageBound));

    std::vector<Spectrum> col(int(fullResolution.x) * int(fullResolution.y));
    std::cout << "Rendering begins! " << "Using " << NUM_PROCS << " cores." << std::endl;

    omp_set_num_threads(NUM_PROCS);
    #pragma omp parallel for
    for (size_t i = 0; i < int(fullResolution.x); i++)
    {
        for (size_t j = 0; j < int(fullResolution.y); j++)
        {
            int offset = (i + (int)fullResolution.x * j);
            std::unique_ptr<Sampler> pixel_sampler = hns->Clone(offset);

            Point2i pixel(i, j);
            pixel_sampler->StartPixel(pixel);
            Spectrum colObj(0.0f);

            do {
                CameraSample cs;
                cs = pixel_sampler->GetCameraSample(pixel);

                Ray r;
                cam->GenerateRay(cs, &r) ;
                float tHit;
                SurfaceInteraction isect;

                if (worldScene->Intersect(r, &isect)) {
                    float Li = Dot(Light, isect.n);
                    Li = Clamp(Li, 0.0, 1.0);
                    Li = sqrt(Li);

                    colObj[0] += Li;
                    colObj[1] += Li;
                    colObj[2] += Li;
                }
            } while (pixel_sampler->StartNextSample());

            colObj /= (float)pixel_sampler->samplesPerPixel;
            col[offset] = colObj;
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