#pragma once

#include <embree4/rtcore.h>

#include <PBRender/core/common.h>
#include <PBRender/core/scene.h>

inline void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
  printf("error %d: %s\n", error, str);
}


namespace PBRender
{

class Engine {
public:
    Engine();
    ~Engine();

    void RenderPixel();
    void RenderTile();

public:
    std::shared_ptr<Scene> scene;

private:
    RTCDevice device;
};


} // namespace PBRender