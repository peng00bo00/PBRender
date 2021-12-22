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

#include "texture.h"
#include "textures/constant.h"

#include "material.h"
#include "materials/matte.h"

#include "light.h"
#include "lights/point.h"

#include <stb_image_write.h>


using namespace PBRender;


std::vector<char> color2Img(std::vector<Spectrum> col) {
    int N = col.size();
    std::vector<char> buf(N * 3);

    for (size_t i = 0; i < N; ++i)
    {
        buf[3 * i + 0] = 255 * Clamp(col[i][0], 0.0f, 1.0f);
        buf[3 * i + 1] = 255 * Clamp(col[i][1], 0.0f, 1.0f);
        buf[3 * i + 2] = 255 * Clamp(col[i][2], 0.0f, 1.0f);
    }
    
    return buf;
}

void test() {

    // textures
    Spectrum floorColor, modelColor;
    floorColor[0] = 0.2; floorColor[1] = 0.3; floorColor[2] = 0.9;
    modelColor[0] = 0.8; modelColor[1] = 0.1; modelColor[2] = 0.2;

    std::shared_ptr<Texture<Spectrum>> KdFloor = std::make_shared<ConstantTexture<Spectrum>>(floorColor);
    std::shared_ptr<Texture<Spectrum>> KdModel = std::make_shared<ConstantTexture<Spectrum>>(modelColor);

    auto sigma   = std::make_shared<ConstantTexture<float>>(0.0f);
    auto bumpMap = std::make_shared<ConstantTexture<float>>(0.0f);

    // materials
    auto modelMaterial = std::make_shared<MatteMaterial>(KdModel, sigma, bumpMap);
    auto floorMaterial = std::make_shared<MatteMaterial>(KdFloor, sigma, bumpMap);

    // initialize worldScene
    Transform Object2WorldModel = Scale( 1.0, 1.0, 1.0 );
    Object2WorldModel = Translate(Vector3f(0.0, 0.0, 1.0)) * Object2WorldModel;
    std::vector<std::shared_ptr<Primitive>> prims;

    ModelLoader loader;
    loader.loadModel("./teapot.obj", Object2WorldModel);
    loader.buildNoTextureModel(Object2WorldModel, prims, modelMaterial);

    // floor
    int nTrianglesFloor = 2;
    int vertexIndicesFloor[6] = { 0 ,1 ,2 ,3 ,4 ,5 };
    int nVerticesFloor = 6;
    const float yPos_Floor = -2.0;

    Point3f P_Floor [6] = {
        Point3f(-16.0, yPos_Floor, 16.0), Point3f(16.0, yPos_Floor, 16.0), Point3f(-16.0, yPos_Floor, -16.0),
        Point3f( 16.0, yPos_Floor, 16.0), Point3f(16.0, yPos_Floor,-16.0), Point3f(-16.0, yPos_Floor, -16.0)
    };

    Transform tri_Object2World2, tri_World2Object2;
    std::shared_ptr<TriangleMesh> meshFloor = std::make_shared<TriangleMesh>(tri_Object2World2, nTrianglesFloor, vertexIndicesFloor,
                                                                             nVerticesFloor, P_Floor, nullptr, nullptr, nullptr, nullptr);
    std::vector<std::shared_ptr<Shape>> trisFloor;

    for(int i = 0 ; i < nTrianglesFloor; ++i)
        trisFloor.push_back(std::make_shared<Triangle>(&tri_Object2World2, &tri_World2Object2, false, meshFloor, i));
    
    for(int i = 0 ; i < nTrianglesFloor; ++i)
        prims.push_back(std::make_shared<GeometricPrimitive>(trisFloor[i], floorMaterial));

    // light
    Spectrum LightI(30.0f);
    Transform LightToWorld;
    LightToWorld = Translate(Vector3f(3.0f, 5.0f,-3.0f)) * LightToWorld;

    auto pointLight = std::make_shared<PointLight>(LightToWorld, LightI);
    std::vector<std::shared_ptr<Light>> lights;
    lights.push_back(pointLight);

    // scene
    std::unique_ptr<Scene> worldScene;
    worldScene = std::make_unique<Scene>(CreateBVHAccelerator(prims), lights);

    // initialize camera
    Point3f eye(0.0f, 5.0f,-3.0f), look(0.0, 0.0, 1.0f);
    Vector3f up(0.0f, 1.0f, 0.0f);

    Transform lookat = LookAt(eye, look, up);
    Transform Camera2World = Inverse(lookat);

    Vector2f fullResolution(800, 600);
    std::shared_ptr<Camera> camera;
    camera = std::shared_ptr<PerspectiveCamera>(CreatePerspectiveCamera(Camera2World, fullResolution, 90.0f));

    // sampler
    Bounds2i imageBound(Point2i(0, 0), Point2i(fullResolution.x, fullResolution.y));
    std::shared_ptr<Sampler> sampler = std::shared_ptr<HaltonSampler>(CreateHaltonSampler(16, imageBound));

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