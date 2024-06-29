#include <PBRender/core/engine.h>


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


void GeometryViewer::RenderPixel(int x, int y, Array2D<Vector3f> &frame) {
    std::cout << "Rendering Pixel: " << Point2i(x, y) << "..." << std::endl;

    CameraSample sample;
    sample.pFilm = Point2f{x + 0.5f, y + 0.5f};
    Ray ray;

    camera->GenerateRay(sample, ray);

    // initialize a rayhit
    RTCRayHit rayhit;
    {
        rayhit.ray.org_x = ray.org.x;
        rayhit.ray.org_y = ray.org.y;
        rayhit.ray.org_z = ray.org.z;
        rayhit.ray.dir_x = ray.dir.x;
        rayhit.ray.dir_y = ray.dir.y;
        rayhit.ray.dir_z = ray.dir.z;

        rayhit.ray.tnear = 0;
        rayhit.ray.tfar  = PBRender::Infinity;

        rayhit.ray.mask  =-1;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
    }

    scene->RayHit(&rayhit);
    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        float albedo[3] = {0.f, 0.f ,0.f};
        const uint albedo_slot = 0;

        uint geomID = rayhit.hit.geomID;
        RTCGeometry geom = rtcGetGeometry(scene->GetScene(), geomID);

        uint primID = rayhit.hit.primID;

        float u = rayhit.hit.u;
        float v = rayhit.hit.v;

        rtcInterpolate0(geom, 
                        primID, 
                        u, v, 
                        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                        albedo_slot,
                        albedo,
                        3);

        frame(x, y) = Vector3f(albedo[0], albedo[1], albedo[2]);
    }
}

void GeometryViewer::RenderTile(const Bounds2i TileBound, Array2D<Vector3f> &frame) {
    for (int x = TileBound.pMin.x; x < TileBound.pMax.x; ++x) {
        for (int y = TileBound.pMin.y; y < TileBound.pMax.y; ++y) {
            RenderPixel(x, y, frame);
        }
    }

    std::cout << "Rendering Tile: " << TileBound << " finished!" << std::endl;
}

void GeometryViewer::RenderFrame(Point2i TileSize, Array2D<Vector3f> &frame) {
    Film *film = camera->GetFilm();
    Point2i fullResolution = film->FullResolution();
    Bounds2i fullFrame     = film->FullFrame();

    // prepare tiles
    int numTileX = std::ceil(fullResolution.x / TileSize.x);
    int numTileY = std::ceil(fullResolution.y / TileSize.y);

    std::vector<Bounds2i> tiles;
    tiles.reserve(numTileX * numTileY);
    
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

    // render each tile
    for (const Bounds2i tile : tiles) {
        RenderTile(tile, frame);
    }

    // tbb::task_arena ta;
    // ta.execute([&] {
    //     tbb::affinity_partitioner affinity;
    // });

    std::cout << "Finish rendering!" << std::endl;
}

} // namespace PBRender