#pragma once

#include <embree4/rtcore.h>

#include <PBRender/core/common.h>
#include <PBRender/core/scene.h>


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

public:
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

};


} // namespace PBRender