#pragma once

#include <embree4/rtcore.h>

#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/blocked_range.h>

#include <PBRender/core/common.h>
#include <PBRender/core/scene.h>
#include <PBRender/core/camera.h>

namespace PBRender
{

class Engine {
public:
    Engine();
    virtual ~Engine();

    void RenderPixel();
    void RenderTile();
    void RenderFrame();

    void InitScene();
    std::shared_ptr<Scene> GetScene() { return scene; };

    void SetCamera(std::shared_ptr<Camera> cam) { camera = cam; };
    std::shared_ptr<Camera> GetCamera() { return camera; };

protected:
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;

private:
    RTCDevice device;

    static void errorFunction(void* userPtr, enum RTCError error, const char* str) {
        printf("error %d: %s\n", error, str);
    }
};

inline std::unique_ptr<Engine> InitEngine() {
    return std::make_unique<PBRender::Engine>();
}

class GeometryViewer : public Engine {
public:
    GeometryViewer() : Engine() {};

    void RenderPixel(int x, int y, Array2D<Vector3f> &frame);
    void RenderTile(const Bounds2i TileBound, Array2D<Vector3f> &frame);
    void RenderFrame(const Point2i TileSize, Array2D<Vector3f> &frame);

private:
    struct PixelGeometry {
        PixelGeometry() = default;

        Vector3f albedo;
        Vector3f normal;
        float depth;
    };

    PixelGeometry RayHitQuery(RTCRayHit &rayhit);
};


} // namespace PBRender