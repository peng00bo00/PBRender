#include <PBRender/core/engine.h>

#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/blocked_range.h>

namespace PBRender
{

Engine::Engine() {
    std::cout << "Initialize Device..." << std::endl;

    // device = rtcNewDevice("start_threads=1,set_affinity=1");
    device = rtcNewDevice("start_threads=1, set_affinity, verbose=2");

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));

    rtcSetDeviceErrorFunction(device, errorFunction, NULL);

    std::cout << "Initialize Device finished!" << std::endl;

    // initialize scene after device
    InitScene();
}

Engine::~Engine() {
    // release the camera
    camera.reset();

    // release the scene
    scene.reset();

    // release the device
    rtcReleaseDevice(device);
    std::cout << "Device released!" << std::endl;
}

void Engine::InitScene() {
    // initialize scene
    scene = std::make_shared<Scene>(device);
}

void Engine::RenderFrame(const Point2i TileSize) {
    std::cout << "Start rendering the frame." << std::endl;
    Film *film = camera->GetFilm();
    std::cout << "Film retrieved." << std::endl;
    Point2i fullResolution = film->FullResolution();
    std::cout << "Full resolution: " << fullResolution << std::endl;
    Bounds2i fullFrame     = film->FullFrameBound();
    std::cout << "Full frame bound: " << fullFrame << std::endl;

    // split the full frame to tiles
    int numTileX = std::ceil(fullResolution.x / TileSize.x);
    int numTileY = std::ceil(fullResolution.y / TileSize.y);

    std::vector<Bounds2i> tiles;
    tiles.reserve(numTileX * numTileY);
    std::cout << "Using " << numTileX * numTileY << " tiles for rendering." << std::endl;
    
    for (size_t i = 0; i < fullResolution.x; i += TileSize.x) {
        for (size_t j = 0; j < fullResolution.y; j += TileSize.y) {
            // tile bound
            Point2i pMin(i, j);
            Point2i pMax(i+TileSize.x, j+TileSize.y);
            Bounds2i bound(pMin, pMax);

            bound = Intersect(bound, fullFrame);
            tiles.emplace_back(bound);
        }
    }

    // render each tile in parallel
    std::cout << "Rendering with TBB multithread!" << std::endl;
    tbb::task_arena ta;
    ta.execute([&] {
        tbb::affinity_partitioner affinity;
        tbb::blocked_range<int> range(0, numTileX * numTileY);
        tbb::parallel_for(
            range,
            [&](const tbb::blocked_range<int> r){
                for (int i=r.begin(); i<r.end(); ++i) {
                    RenderTile(tiles[i]);
                }
            },
            affinity
            );
    });
    std::cout << "Finish rendering!" << std::endl;
}

void Engine::RenderTile(const Bounds2i TileBound) {
    for (int x = TileBound.pMin.x; x < TileBound.pMax.x; ++x) {
        for (int y = TileBound.pMin.y; y < TileBound.pMax.y; ++y) {
            RenderPixel(x, y);
        }
    }
}

void RayTracer::RenderPixel(int x, int y) {
    // initialize a ray at film (x, y)
    CameraSample cs;
    cs.pFilm = Point2f{x + 0.5f, y + 0.5f};
    Ray ray;

    camera = GetCamera();
    camera->GenerateRay(cs, ray);

    // initialize a rayhit
    RTCRayHit rayhit;
    InitRTCRayHit(ray, rayhit);

    scene->RayHit(&rayhit);
    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        // retrieve geometry
        uint geomID = rayhit.hit.geomID;
        RTCGeometry geom = scene->GetGeometry(geomID);

        // primID and (u, v) coordinate
        uint primID = rayhit.hit.primID;
        float u = rayhit.hit.u;
        float v = rayhit.hit.v;

        // albedo
        float albedo[3] = {0.f, 0.f ,0.f};

        rtcInterpolate0(geom, 
                    primID, 
                    u, v, 
                    RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                    VERTEX_ATTRIB_SLOT::VERTEX_ALBEDO,
                    albedo,
                    3);

        // Blinn-Phong shader
        Spectrum L;

        // diffuse
        for (size_t i=0; i<3; ++i)
            L[i] += 0.5f * albedo[i];

        // specular
        Vector3f lightDir(0.f, 1.f, 0.f); lightDir = Normalize(lightDir);
        Vector3f viewDir = -ray.dir; viewDir = Normalize(viewDir);
        Vector3f halfwayDir = Normalize(lightDir + viewDir);

        Vector3f normal(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
        normal = Normalize(normal);

        float spec = std::max(Dot(normal, halfwayDir), 0.f);
        for (size_t i=0; i<3; ++i)
            L[i] += spec * albedo[i];

        // write to film
        auto film = camera->GetFilm();
        Point2i pFilm(x, y);
        film->AddSample(pFilm, L, 1.f);
    }
}

void GeometryViewer::RenderPixel(int x, int y) {
    // initialize a ray at film (x, y)
    CameraSample cs;
    cs.pFilm = Point2f{x + 0.5f, y + 0.5f};
    Ray ray;

    camera = GetCamera();
    camera->GenerateRay(cs, ray);

    // initialize a rayhit
    RTCRayHit rayhit;
    InitRTCRayHit(ray, rayhit);

    scene->RayHit(&rayhit);
    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        PixelGeometry pixel = RayHitQuery(rayhit);
        Spectrum L;

        switch (gImg) {
            case GeometryImage::NORMAL:
                // normalize normal vector to (0, 1)
                for (size_t i = 0; i < 3; ++i)
                    L[i] = pixel.normal[i] * 0.5f + 0.5f;
                
                break;
            case GeometryImage::DEPTH:
                L = pixel.depth;
                break;
            default:
                L = pixel.albedo;
        }

        // write to film
        auto film = camera->GetFilm();
        Point2i pFilm(x, y);
        film->AddSample(pFilm, L, 1.f);
    }
}


GeometryViewer::PixelGeometry GeometryViewer::RayHitQuery(RTCRayHit &rayhit) {
    // retrieve geometry
    uint geomID = rayhit.hit.geomID;
    RTCGeometry geom = scene->GetGeometry(geomID);

    // primID and (u, v) coordinate
    uint primID = rayhit.hit.primID;
    float u = rayhit.hit.u;
    float v = rayhit.hit.v;

    float albedo[3] = {0.f, 0.f ,0.f};

    // iterpolate
    rtcInterpolate0(geom, 
                    primID, 
                    u, v, 
                    RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                    VERTEX_ATTRIB_SLOT::VERTEX_ALBEDO,
                    albedo,
                    3);
    
    GeometryViewer::PixelGeometry pixel;

    // albedo
    pixel.albedo = Vector3f(albedo[0], 
                            albedo[1], 
                            albedo[2]);

    // normal
    pixel.normal = Vector3f(rayhit.hit.Ng_x, 
                            rayhit.hit.Ng_y,
                            rayhit.hit.Ng_z);

    // depth
    pixel.depth  = Vector3f(rayhit.ray.tfar,
                            rayhit.ray.tfar,
                            rayhit.ray.tfar);
    
    return pixel;
}

} // namespace PBRender