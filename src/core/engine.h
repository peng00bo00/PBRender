#pragma once

#include <embree4/rtcore.h>

#include "common.h"
#include "scene.h"

void errorFunction(void* userPtr, enum RTCError error, const char* str)
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

Engine::Engine() {
    std::cout << "initialize Device..." << std::endl;

    device = rtcNewDevice("start_threads=1,set_affinity=1");

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));

    rtcSetDeviceErrorFunction(device, errorFunction, NULL);

    std::cout << "initialize Device finished!" << std::endl;

    // initialize scene
    scene = std::make_shared<Scene>(device);    
}

Engine::~Engine() {
    // release the scene
    scene.reset();

    std::cout << "release Device..." << std::endl;

    rtcReleaseDevice(device);

    std::cout << "Device released!" << std::endl;
}

} // namespace PBRender