#include <PBRender/core/engine.h>


namespace PBRender
{

Engine::Engine() {
    std::cout << "Initialize Device..." << std::endl;

    // device = rtcNewDevice("start_threads=1,set_affinity=1");
    device = rtcNewDevice(nullptr);

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));

    rtcSetDeviceErrorFunction(device, errorFunction, NULL);

    std::cout << "Initialize Device finished!" << std::endl;

    // initialize scene
    scene = std::make_shared<Scene>(device);
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

} // namespace PBRender