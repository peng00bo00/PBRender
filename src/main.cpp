#include <PBRender/PBRender.h>

using Point2i = PBRender::Point2i;
using Point2f = PBRender::Point2f;
using Point3f = PBRender::Point3f;
using Vector3i= PBRender::Vector3i;
using Vector3f= PBRender::Vector3f;

// initialize cornel box scene
void InitCornellBox(PBRender::Scene *scene) {
    std::vector<Point3f> vertices;
    std::vector<Vector3i> indices;
    size_t geomID;
    Vector3f albedo;

    // floor
    vertices.clear(); indices.clear();

    vertices.emplace_back(-1, 0,-1);
    vertices.emplace_back(-1, 0, 1);
    vertices.emplace_back( 1, 0, 1);
    vertices.emplace_back( 1, 0,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);
    
    albedo = Vector3f(0.725f, 0.71f, 0.68f);
    geomID = scene->AddTriMesh(vertices, indices, albedo);

    // ceiling
    vertices.clear(); indices.clear();

    vertices.emplace_back( 1, 2, 1);
    vertices.emplace_back(-1, 2, 1);
    vertices.emplace_back(-1, 2,-1);
    vertices.emplace_back( 1, 2,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    albedo = Vector3f(0.725f, 0.71f, 0.68f);
    geomID = scene->AddTriMesh(vertices, indices, albedo);

    // back wall
    vertices.clear(); indices.clear();

    vertices.emplace_back(-1, 0,-1);
    vertices.emplace_back(-1, 2,-1);
    vertices.emplace_back( 1, 2,-1);
    vertices.emplace_back( 1, 0,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    albedo = Vector3f(0.725f, 0.71f, 0.68f);
    geomID = scene->AddTriMesh(vertices, indices, albedo);

    // right wall
    vertices.clear(); indices.clear();

    vertices.emplace_back( 1, 0,-1);
    vertices.emplace_back( 1, 2,-1);
    vertices.emplace_back( 1, 2, 1);
    vertices.emplace_back( 1, 0, 1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    albedo = Vector3f(0.14f, 0.45f, 0.091f);
    geomID = scene->AddTriMesh(vertices, indices, albedo);

    // left wall
    vertices.clear(); indices.clear();

    vertices.emplace_back(-1, 0, 1);
    vertices.emplace_back(-1, 2, 1);
    vertices.emplace_back(-1, 2,-1);
    vertices.emplace_back(-1, 0,-1);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    albedo = Vector3f(0.63f, 0.065f, 0.05f);
    geomID = scene->AddTriMesh(vertices, indices, albedo);

    // light
    vertices.clear(); indices.clear();

    vertices.emplace_back(-0.24f, 1.98f, -0.22f);
    vertices.emplace_back( 0.23f, 1.98f, -0.22f);
    vertices.emplace_back( 0.23f, 1.98f,  0.16f);
    vertices.emplace_back(-0.24f, 1.98f,  0.16f);

    indices.emplace_back(0, 1, 2);
    indices.emplace_back(0, 2, 3);

    albedo = Vector3f(17.f, 12.f, 4.f) / 255.f;
    geomID = scene->AddTriMesh(vertices, indices, albedo);


    // short box
    vertices.clear(); indices.clear();

    vertices.emplace_back(-0.0460751, 0.6, 0.573007);
    vertices.emplace_back(-0.0460751, -2.98023e-8, 0.573007);
    vertices.emplace_back(0.124253, 0, 0.00310463);
    vertices.emplace_back(0.124253, 0.6, 0.00310463);
    vertices.emplace_back(0.533009, 0, 0.746079);
    vertices.emplace_back(0.533009, 0.6, 0.746079);
    vertices.emplace_back(0.703337, 0.6, 0.176177);
    vertices.emplace_back(0.703337, 2.98023e-8, 0.176177);
    vertices.emplace_back(0.533009, 0.6, 0.746079);
    vertices.emplace_back(-0.0460751, 0.6, 0.573007);
    vertices.emplace_back(0.124253, 0.6, 0.00310463);
    vertices.emplace_back(0.703337, 0.6, 0.176177);
    vertices.emplace_back(0.703337, 2.98023e-8, 0.176177);
    vertices.emplace_back(0.124253, 0, 0.00310463);
    vertices.emplace_back(-0.0460751, -2.98023e-8, 0.573007);
    vertices.emplace_back(0.533009, 0, 0.746079);
    vertices.emplace_back(0.533009, 0, 0.746079);
    vertices.emplace_back(-0.0460751, -2.98023e-8, 0.573007);
    vertices.emplace_back(-0.0460751, 0.6, 0.573007);
    vertices.emplace_back(0.533009, 0.6, 0.746079);
    vertices.emplace_back(0.703337, 0.6, 0.176177);
    vertices.emplace_back(0.124253, 0.6, 0.00310463);
    vertices.emplace_back(0.124253, 0, 0.00310463);
    vertices.emplace_back(0.703337, 2.98023e-8, 0.176177);

    indices.emplace_back(0, 2, 1);
    indices.emplace_back(0, 3, 2);
    indices.emplace_back(4, 6, 5);
    indices.emplace_back(4, 7, 6);
    indices.emplace_back(8, 10, 9);
    indices.emplace_back(8, 11, 10);
    indices.emplace_back(12, 14, 13);
    indices.emplace_back(12, 15, 14);
    indices.emplace_back(16, 18, 17);
    indices.emplace_back(16, 19, 18);
    indices.emplace_back(20, 22, 21);
    indices.emplace_back(20, 23, 22);

    albedo = Vector3f(0.725f, 0.71f, 0.68f);
    geomID = scene->AddTriMesh(vertices, indices, albedo);

    // tall box
    vertices.clear(); indices.clear();

    vertices.emplace_back(-0.720444, 1.2, -0.473882);
    vertices.emplace_back(-0.720444, 0, -0.473882);
    vertices.emplace_back(-0.146892, 0, -0.673479);
    vertices.emplace_back(-0.146892, 1.2, -0.673479);
    vertices.emplace_back(-0.523986, 0, 0.0906493);
    vertices.emplace_back(-0.523986, 1.2, 0.0906492);
    vertices.emplace_back(0.0495656, 1.2, -0.108948);
    vertices.emplace_back(0.0495656, 0, -0.108948);
    vertices.emplace_back(-0.523986, 1.2, 0.0906492);
    vertices.emplace_back(-0.720444, 1.2, -0.473882);
    vertices.emplace_back(-0.146892, 1.2, -0.673479);
    vertices.emplace_back(0.0495656, 1.2, -0.108948);
    vertices.emplace_back(0.0495656, 0, -0.108948);
    vertices.emplace_back(-0.146892, 0, -0.673479);
    vertices.emplace_back(-0.720444, 0, -0.473882);
    vertices.emplace_back(-0.523986, 0, 0.0906493);
    vertices.emplace_back(-0.523986, 0, 0.0906493);
    vertices.emplace_back(-0.720444, 0, -0.473882);
    vertices.emplace_back(-0.720444, 1.2, -0.473882);
    vertices.emplace_back(-0.523986, 1.2, 0.0906492);
    vertices.emplace_back(0.0495656, 1.2, -0.108948);
    vertices.emplace_back(-0.146892, 1.2, -0.673479);
    vertices.emplace_back(-0.146892, 0, -0.673479);
    vertices.emplace_back(0.0495656, 0, -0.108948);

    indices.emplace_back(0, 2, 1);
    indices.emplace_back(0, 3, 2);
    indices.emplace_back(4, 6, 5);
    indices.emplace_back(4, 7, 6);
    indices.emplace_back(8, 10, 9);
    indices.emplace_back(8, 11, 10);
    indices.emplace_back(12, 14, 13);
    indices.emplace_back(12, 15, 14);
    indices.emplace_back(16, 18, 17);
    indices.emplace_back(16, 19, 18);
    indices.emplace_back(20, 22, 21);
    indices.emplace_back(20, 23, 22);

    albedo = Vector3f(0.725f, 0.71f, 0.68f);
    geomID = scene->AddTriMesh(vertices, indices, albedo);
}

int main() {
    // create engine instance    
    // PBRender::GeometryViewer::GeometryImage gImg{PBRender::GeometryViewer::GeometryImage::ALBEDO};
    // auto engine = PBRender::InitGeometryViewer(gImg);

    auto engine = PBRender::InitRayTracer();

    // create a film
    const int W = 1024;
    const int H = 1024;

    Point2i fullResolution(W, H);
    std::string file_name = "Cornel Box Geometry";
    // PBRender::GBuffferFilm film(fullResolution, file_name);
    PBRender::RGBFilm film(fullResolution, file_name);

    // set up camera
    Point3f pos(0.f, 1.f, 6.8f);
    Point3f look(0.f, 1.f, 0.f);
    Vector3f up(0.f, 1.f, 0.f);

    PBRender::Transform world2camera = PBRender::LookAt(pos, look, up);
    PBRender::Transform camera2world = world2camera.Inverse();

    float fov = 19.5f;
    // auto camera = PBRender::CreatePerspectiveCamera(camera2world, &gFilm, fov);
    // auto camera = PBRender::CreateOrthographicCamera(camera2world, &gFilm);

    engine->SetCamera(PBRender::CreatePerspectiveCamera(camera2world, &film, fov));
    
    // set sampler
    const uint spp = 16;
    // auto sampler = std::make_shared<PBRender::IndependentSampler>(spp);
    // auto sampler = std::make_shared<PBRender::HaltonSampler>(spp, fullResolution);

    PBRender::RandomizeStrategy randomizer = PBRender::RandomizeStrategy::FastOwen;
    // auto sampler = std::make_shared<PBRender::SobolSampler>(spp, fullResolution, randomizer);
    // auto sampler = std::make_shared<PBRender::PaddedSobolSampler>(spp, randomizer);
    auto sampler = std::make_shared<PBRender::ZSobolSampler>(spp, fullResolution, randomizer);
    std::cout << sampler->ToString() << std::endl;

    engine->SetSampler(sampler);

    // set up scene
    auto scene = engine->GetScene();
    InitCornellBox(scene.get());

    // finish scene and build BVH
    scene->FinishScene();

    // set up rendering tiles
    Point2i TileSize(128, 128);

    // start rendering
    engine->RenderFrame(TileSize);

    // write to image
    film.WriteImage();
}