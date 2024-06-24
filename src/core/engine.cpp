#include <PBRender/core/engine.h>


namespace PBRender
{

Engine::Engine() {
    std::cout << "initialize Device..." << std::endl;

    // device = rtcNewDevice("start_threads=1,set_affinity=1");
    device = rtcNewDevice(nullptr);

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