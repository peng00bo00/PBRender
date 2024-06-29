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

void RenderPixel(int x, int y, std::vector<float> &frame) {
    std::cout << "Rendering Pixel: " << Point2i(x, y) << "..." << std::endl;
}

void RenderTile(Bounds2i TileBound, std::vector<float> &frame) {
    std::cout << "Rendering Tile: " << TileBound << "..." << std::endl;

    for (int x = TileBound.pMin.x; x < TileBound.pMax.x; ++x) {
        for (int y = TileBound.pMin.y; y < TileBound.pMax.y; ++y) {
            RenderPixel(x, y, frame);
        }
    }

    std::cout << "Rendering Tile: " << TileBound << " finished!" << std::endl;
}

void GeometryViewer::RenderFrame(Point2i TileSize, std::vector<float> &frame) {
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

            std::cout << Point2i(i, j) << ": " << bound << std::endl;
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