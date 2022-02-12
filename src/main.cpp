#include "PBRender.h"

#include "geometry.h"
#include "transform.h"
#include "modelloader.h"

#include "accelerators/bvh.h"
#include "cameras/orthographic.h"
#include "cameras/perspective.h"

#include "sampler.h"
#include "samplers/halton.h"

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "spectrum.h"

#include "texture.h"
#include "textures/constant.h"
#include "textures/imagemap.h"

#include "material.h"
#include "materials/matte.h"
#include "materials/mirror.h"
#include "materials/glass.h"
#include "materials/plastic.h"
#include "materials/metal.h"

#include "mipmap.h"

#include "light.h"
#include "lights/point.h"
#include "lights/diffuse.h"
#include "lights/infinite.h"

#include "scene.h"

#include "integrator.h"
#include "integrators/whitted.h"
#include "integrators/directlighting.h"
#include "integrators/path.h"

#include "imageio.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <stb_image_write.h>

#include "omp.h"


using namespace PBRender;

char TO_BYTE(float v) {
    return (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f);
}

void color2Img(std::vector<Spectrum> &col, std::vector<char> &buf) {

    #pragma omp simd
    for (size_t i = 0; i < col.size(); ++i)
    {
        buf[3 * i + 0] = TO_BYTE(col[i][0]);
        buf[3 * i + 1] = TO_BYTE(col[i][1]);
        buf[3 * i + 2] = TO_BYTE(col[i][2]);
    }
}

void test() {

    // textures
    Spectrum floorColor, modelColor;
    floorColor[0] = 0.8; floorColor[1] = 0.8; floorColor[2] = 0.8;
    modelColor[0] = 0.8; modelColor[1] = 0.8; modelColor[2] = 0.8;

    std::shared_ptr<Texture<Spectrum>> KdFloor = std::make_shared<ConstantTexture<Spectrum>>(floorColor);
    std::shared_ptr<Texture<Spectrum>> KdModel = std::make_shared<ConstantTexture<Spectrum>>(modelColor);

    auto sigma   = std::make_shared<ConstantTexture<float>>(0.0f);
    auto bumpMap = std::make_shared<ConstantTexture<float>>(0.0f);

    // materials
    auto modelMaterial = std::make_shared<MatteMaterial>(KdModel, sigma, bumpMap);
    auto floorMaterial = std::make_shared<MatteMaterial>(KdFloor, sigma, bumpMap);

    Spectrum mirrorColor(1.f);
    std::shared_ptr<Texture<Spectrum>> KrMirror = std::make_shared<ConstantTexture<Spectrum>>(mirrorColor);
    std::shared_ptr<Material> mirrorMaterial = std::make_shared<MirrorMaterial>(KrMirror , bumpMap);

    // auto glassEta = std::make_shared<ConstantTexture<float>>(1.2f);
    // auto glassKr  = std::make_shared<ConstantTexture<Spectrum>>(1.0f);
    // auto glassKt  = std::make_shared<ConstantTexture<Spectrum>>(1.0f);

    // auto glassMaterial = std::make_shared<PerfectGlassMaterial>(glassKr, glassKt, glassEta, bumpMap);

    // glass
    Spectrum c1;
    c1[0] = 0.98f; c1[1] = 0.98f; c1[2] = 0.98f;
    // c1[0] = 0.18f; c1[1] = 0.15f; c1[2] = 0.81f;
    auto Kr = std::make_shared<ConstantTexture<Spectrum>>(c1);

    Spectrum c2;
    // c2[0] = 0.98f; c2[1] = 0.98f; c2[2] = 0.98f;
    c2[0] = 0.18f; c2[1] = 0.15f; c2[2] = 0.81f;
    auto Kt = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0f) - c2);

    auto index = std::make_shared<ConstantTexture<float>>(1.5f);

    auto RoughnessU = std::make_shared<ConstantTexture<float>>(0.5f);
    auto RoughnessV = std::make_shared<ConstantTexture<float>>(0.5f);

    auto glassMaterial = std::make_shared<GlassMaterial>(Kr, Kt, RoughnessU, RoughnessV, index, bumpMap, false);

    // plastic
    Spectrum plasticK;
    // plasticK[0] = 0.18; plasticK[1] = 0.15; plasticK[2] = 0.81;
    plasticK[0] = 0.98; plasticK[1] = 0.98; plasticK[2] = 0.98;
    auto plasticKd = std::make_shared<ConstantTexture<Spectrum>>(plasticK);
    auto plasticKr = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0f) - plasticK);
    auto plasticRoughness = std::make_shared<ConstantTexture<float>>(0.1f);

    auto plasticMaterial = std::make_shared<PlasticMaterial>(plasticKd, plasticKr, plasticRoughness, bumpMap, true);

    // metal
    Spectrum eta;
    // eta[0] = 0.18f; eta[1] = 0.15f; eta[2] = 0.81f;
    eta[0] = 0.02f; eta[1] = 0.02f; eta[2] = 0.02f;
    auto etaM = std::make_shared<ConstantTexture<Spectrum>>(eta);

    Spectrum k;
    k[0] = 0.11f; k[1] = 0.11f; k[2] = 0.11f;
    auto kM = std::make_shared<ConstantTexture<Spectrum>>(k);

    auto roughness  = std::make_shared<ConstantTexture<float>>(0.01f);
    auto uRoughness = std::make_shared<ConstantTexture<float>>(0.1f);
    auto vRoughness = std::make_shared<ConstantTexture<float>>(0.1f);

    auto metalMaterial = std::make_shared<MetalMaterial>(etaM , kM,
                            roughness, uRoughness, vRoughness, bumpMap, false);

    // texture
    Spectrum LeftWall, RightWall, Floor, Ceiling, BackWall;
    LeftWall[0]  = 0.63; LeftWall[1]  =0.065; LeftWall[2]  = 0.05;
    RightWall[0] = 0.14; RightWall[1] = 0.45; RightWall[2] = 0.09;
    BackWall[0]  = 0.725;BackWall[1]  = 0.71; BackWall[2]  = 0.68;

    Floor[0]   = 0.725; Floor[1]   = 0.71; Floor[2]   = 0.68;
    Ceiling[0] = 0.725; Ceiling[1] = 0.71, Ceiling[2] = 0.68;

    auto LeftWallTexture  = std::make_shared<ConstantTexture<Spectrum>>(LeftWall);
    auto RightWallTexture = std::make_shared<ConstantTexture<Spectrum>>(RightWall);
    auto FloorTexture     = std::make_shared<ConstantTexture<Spectrum>>(Floor);
    auto CeilingTexture   = std::make_shared<ConstantTexture<Spectrum>>(Ceiling);
    auto BackWallTexture  = std::make_shared<ConstantTexture<Spectrum>>(BackWall);

    // materials
    // auto sigma   = std::make_shared<ConstantTexture<float>>(0.0f);
    // auto bumpMap = std::make_shared<ConstantTexture<float>>(0.0f);
    
    auto LeftWallMaterial = std::make_shared<MatteMaterial>(LeftWallTexture, sigma, bumpMap);
    auto RightWallMaterial= std::make_shared<MatteMaterial>(RightWallTexture, sigma, bumpMap);
    auto FloorMaterial    = std::make_shared<MatteMaterial>(FloorTexture, sigma, bumpMap);
    auto CeilingMaterial  = std::make_shared<MatteMaterial>(CeilingTexture, sigma, bumpMap);
    auto BackWallMaterial = std::make_shared<MatteMaterial>(BackWallTexture, sigma, bumpMap);

    std::vector<std::shared_ptr<Shape>> trisFloor;
    std::vector<std::shared_ptr<Primitive>> prims;

    // CornellBox
    int nTrianglesFloor = 2;
    int vertexIndicesFloor[6] = {0, 1, 2, 3, 4, 5};
    int nVerticesFloor = 6;

    float length_Floor = 5.0f;
    int offset = 0;

    Transform tri_Object2World2, tri_World2Object2;

    // Floor
    Point3f P_Floor[6] = {
        Point3f(0.f, 0.f, length_Floor), Point3f(length_Floor, 0.f, length_Floor), Point3f(0.f, 0.f, 0.f),
        Point3f(length_Floor, 0.f, length_Floor), Point3f(length_Floor, 0.f, 0.f), Point3f(0.f, 0.f, 0.f)
    };

    std::shared_ptr<TriangleMesh> meshFloor = std::make_shared<TriangleMesh>(tri_Object2World2, nTrianglesFloor, vertexIndicesFloor,
                                                                             nVerticesFloor, P_Floor, nullptr, nullptr, nullptr, nullptr);

    for(int i = 0 ; i < nTrianglesFloor; ++i)
        trisFloor.push_back(std::make_shared<Triangle>(&tri_Object2World2, &tri_World2Object2, false, meshFloor, i));
    
    for(int i = 0 ; i < nTrianglesFloor; ++i)
        prims.push_back(std::make_shared<GeometricPrimitive>(trisFloor[i+offset], FloorMaterial, nullptr));
    
    offset += nTrianglesFloor;
    
    // Ceiling
    Point3f P_Ceiling[6] = {
        Point3f(0.f, length_Floor, length_Floor), Point3f(0.f, length_Floor, 0.f), Point3f(length_Floor, length_Floor, length_Floor),
        Point3f(length_Floor, length_Floor, length_Floor), Point3f(0.f, length_Floor, 0.f), Point3f(length_Floor, length_Floor, 0.f),
    };

    meshFloor = std::make_shared<TriangleMesh>(tri_Object2World2, nTrianglesFloor, vertexIndicesFloor,
                                               nVerticesFloor, P_Ceiling, nullptr, nullptr, nullptr, nullptr);

    for(int i = 0 ; i < nTrianglesFloor; ++i)
        trisFloor.push_back(std::make_shared<Triangle>(&tri_Object2World2, &tri_World2Object2, false, meshFloor, i));
    
    for(int i = 0 ; i < nTrianglesFloor; ++i)
        prims.push_back(std::make_shared<GeometricPrimitive>(trisFloor[i+offset], CeilingMaterial, nullptr));
    
    offset += nTrianglesFloor;
    
    // BackWall
    Point3f P_BackWall[6] = {
        Point3f(0.f, 0.f, 0.f), Point3f(length_Floor, 0.f, 0.f), Point3f(length_Floor, length_Floor, 0.f),
        Point3f(0.f, 0.f, 0.f), Point3f(length_Floor, length_Floor, 0.f), Point3f(0.f, length_Floor, 0.f),
    };

    meshFloor = std::make_shared<TriangleMesh>(tri_Object2World2, nTrianglesFloor, vertexIndicesFloor,
                                               nVerticesFloor, P_BackWall, nullptr, nullptr, nullptr, nullptr);

    for(int i = 0 ; i < nTrianglesFloor; ++i)
        trisFloor.push_back(std::make_shared<Triangle>(&tri_Object2World2, &tri_World2Object2, false, meshFloor, i));
    
    for(int i = 0 ; i < nTrianglesFloor; ++i)
        prims.push_back(std::make_shared<GeometricPrimitive>(trisFloor[i+offset], BackWallMaterial, nullptr));
    
    offset += nTrianglesFloor;
    
    // RightWall
    Point3f P_RightWall[6] = {
        Point3f(0.f, 0.f, 0.f), Point3f(0.f, length_Floor, length_Floor), Point3f(0.f, 0.f, length_Floor),
        Point3f(0.f, 0.f, 0.f), Point3f(0.f, length_Floor, 0.f), Point3f(0.f, length_Floor, length_Floor),
    };

    meshFloor = std::make_shared<TriangleMesh>(tri_Object2World2, nTrianglesFloor, vertexIndicesFloor,
                                               nVerticesFloor, P_RightWall, nullptr, nullptr, nullptr, nullptr);

    for(int i = 0 ; i < nTrianglesFloor; ++i)
        trisFloor.push_back(std::make_shared<Triangle>(&tri_Object2World2, &tri_World2Object2, true, meshFloor, i));
    
    for(int i = 0 ; i < nTrianglesFloor; ++i)
        prims.push_back(std::make_shared<GeometricPrimitive>(trisFloor[i+offset], RightWallMaterial, nullptr));
    
    offset += nTrianglesFloor;
    
    // LeftWall
    Point3f P_LeftWall[6] = {
        Point3f(length_Floor, 0.f, 0.f), Point3f(length_Floor, length_Floor,length_Floor), Point3f(length_Floor, 0.f, length_Floor),
        Point3f(length_Floor, 0.f, 0.f), Point3f(length_Floor, length_Floor, 0.f), Point3f(length_Floor, length_Floor, length_Floor)
    };

    meshFloor = std::make_shared<TriangleMesh>(tri_Object2World2, nTrianglesFloor, vertexIndicesFloor,
                                               nVerticesFloor, P_LeftWall, nullptr, nullptr, nullptr, nullptr);

    for(int i = 0 ; i < nTrianglesFloor; ++i)
        trisFloor.push_back(std::make_shared<Triangle>(&tri_Object2World2, &tri_World2Object2, false, meshFloor, i));
    
    for(int i = 0 ; i < nTrianglesFloor; ++i)
        prims.push_back(std::make_shared<GeometricPrimitive>(trisFloor[i+offset], LeftWallMaterial, nullptr));
    
    offset += nTrianglesFloor;
    
    std::cout << "Finish background!" << std::endl;

    // model
    Transform Object2WorldModel = Scale( 15.0, 15.0, 15.0 );
    Object2WorldModel = Translate(Vector3f(length_Floor/2+0.2, 0, 2.0)) * Object2WorldModel;

    ModelLoader loader;
    loader.loadModel("./bunny.obj", Object2WorldModel);
    loader.buildNoTextureModel(Object2WorldModel, prims, plasticMaterial);

    std::cout << "Finish model loading!" << std::endl;
    
    // light
    std::vector<std::shared_ptr<Light>> lights;
    std::vector<std::shared_ptr<AreaLight>> areaLights;
    int nTrianglesAreaLight = 2;
    int vertexIndicesAreaLight[6] = {0, 1, 2, 3, 4, 5};
    int nVerticesAreaLight = 6;
    const float yPos_AreaLight = 5.0;

    Point3f P_AreaLight[6] = {
        Point3f(-0.4, 0.f, 0.4), Point3f(-0.4, 0.f,-0.4), Point3f(0.4, 0.f, 0.4),
        Point3f( 0.4, 0.f, 0.4), Point3f(-0.4, 0.f,-0.4), Point3f(0.4, 0.f,-0.4)
    };

    Transform tri_Object2World_AreaLight;
    tri_Object2World_AreaLight = Translate(Vector3f(length_Floor/2, length_Floor-0.01, length_Floor/2)) * tri_Object2World_AreaLight;
    Transform tri_World2Object_AreaLight = Inverse(tri_Object2World_AreaLight);

    auto meshAreaLight = std::make_shared<TriangleMesh>(tri_Object2World_AreaLight, nTrianglesAreaLight, vertexIndicesAreaLight,
                                                        nVerticesAreaLight, P_AreaLight, nullptr, nullptr, nullptr, nullptr);
    
    std::vector<std::shared_ptr<Shape>> trisAreaLight;
    for (size_t i = 0; i < nTrianglesAreaLight; ++i) {
        trisAreaLight.push_back(
            std::make_shared<Triangle>(&tri_Object2World_AreaLight, &tri_World2Object_AreaLight, false, meshAreaLight, i)
        );
    }

    std::shared_ptr<AreaLight> area;
    for (size_t i = 0; i < nTrianglesAreaLight; i++) {
        area = std::make_shared<DiffuseAreaLight>(tri_Object2World_AreaLight,
                                                  Spectrum(30.0f),
                                                  5,
                                                  trisAreaLight[i],
                                                  false);
        lights.push_back(area);
        prims.push_back(std::make_shared<GeometricPrimitive>(trisAreaLight[i],
                                                             floorMaterial,
                                                             area));
    }

    std::cout << "Finish light!" << std::endl;

    // scene
    std::unique_ptr<Scene> worldScene;
    worldScene = std::make_unique<Scene>(CreateBVHAccelerator(prims), lights);

    // initialize camera
    Point3f eye(2.5f, 2.5f, 6.0f), look(2.5, 2.5, 0.0f);
    Vector3f up(0.0f, 1.0f, 0.0f);

    Transform lookat = LookAt(eye, look, up);
    Transform Camera2World = Inverse(lookat);

    Vector2f fullResolution(800, 800);
    std::shared_ptr<Camera> camera;
    camera = std::shared_ptr<PerspectiveCamera>(CreatePerspectiveCamera(Camera2World, fullResolution));

    // sampler
    Bounds2i imageBound(Point2i(0, 0), Point2i(fullResolution.x, fullResolution.y));
    std::shared_ptr<Sampler> sampler = std::shared_ptr<HaltonSampler>(CreateHaltonSampler(64, imageBound));

    std::vector<Spectrum> col(int(fullResolution.x) * int(fullResolution.y));

    // integrator
    // auto integrator = std::make_shared<WhittedIntegrator>(128, camera, sampler, imageBound);
    
    // auto integrator = std::make_shared<DirectLightingIntegrator>(LightStrategy::UniformSampleAll,
    //                                                              64,
    //                                                              camera,
    //                                                              sampler,
    //                                                              imageBound);

    auto integrator = std::make_shared<PathIntegrator>(256,
                                                       camera,
                                                       sampler,
                                                       imageBound);

    std::cout << "Start rendering!" << std::endl;
    // integrator->Render(*worldScene, col);

    // rendering with openmp for now
    integrator->Preprocess(*worldScene, *sampler);

    // omp_set_num_threads(omp_get_num_procs());
    omp_set_num_threads(8);
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (size_t i = 0; i < (int)fullResolution.x; ++i) {
        for (size_t j = 0; j < (int)fullResolution.y; ++j) {

            int offset = (i + fullResolution.x * j);
            auto colObj = integrator->RenderPixel(*worldScene, i, j);

            col[offset] = colObj;
        }
    }


    auto buf = std::vector<char>();
    buf.resize(3 * col.size());

    color2Img(col, buf);

    // stbi_write_png("test.png", int(fullResolution.x), int(fullResolution.y), 3, buf.data(), 0);
    WriteImage("test.png", buf.data(), fullResolution);
}

int main(int argc, char *argv[]) {
    // int mW = 400, mH = 235;
    // Spectrum *mpdata = new Spectrum[mW * mH];
    // for (int i = 0; i < mW; ++i) {
    //     for (int j = 0; j < mH; ++j) {
    //         int offset = i + j * mW;

    //         Spectrum s;
    //         s[0] = i / (float) 800;
    //         s[1] = j / (float) 800;
    //         s[2] = 0.4f;

    //         mpdata[offset] = s;
    //     }
    // }

    // MIPMap<Spectrum> mp(Point2i(mW, mH), mpdata, true);

    // int RasterWidth = 800;
    // int RasterHeight= 600;
    // int N = RasterWidth * RasterHeight;

    // std::vector<Spectrum> col(N);

    // for (size_t i = 0; i < RasterWidth; ++i) {
    //     for (size_t j = 0; j < RasterHeight; ++j) {
    //         int offset = (i + RasterWidth * j);
    //         auto colObj = mp.Lookup(Point2f((i+1) / (float) RasterWidth, (j+1) / 
    //                         (float) RasterHeight), 0.15f);

    //         col[offset] = colObj;
    //     }
    // }

    // auto buf = color2Img(col);
    // stbi_write_png("test2.png", RasterWidth, RasterHeight, 3, buf.data(), 0);

    // delete[] mpdata;

    test();
    std::cout << "Finish!" << std::endl;

    return 0;
}