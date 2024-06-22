#pragma once

#include <embree4/rtcore.h>

#include <PBRender/core/common.h>
#include <PBRender/core/scene.h>


namespace PBRender
{

class Engine {
public:
    Engine();
    ~Engine();

    void RenderPixel();
    void RenderTile();

    std::shared_ptr<Scene> GetScene() { return scene; };

public:
    std::shared_ptr<Scene> scene;

private:
    RTCDevice device;

    static void errorFunction(void* userPtr, enum RTCError error, const char* str) {
        printf("error %d: %s\n", error, str);
    }
};

inline std::unique_ptr<Engine> InitEngine() {
    return std::make_unique<PBRender::Engine>();
}


} // namespace PBRender