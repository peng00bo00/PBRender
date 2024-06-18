#include <embree4/rtcore.h>
#include <limits>
#include <iostream>


void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
  printf("error %d: %s\n", error, str);
}

RTCDevice initializeDevice() {
    RTCDevice device = rtcNewDevice(NULL);

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));

    rtcSetDeviceErrorFunction(device, errorFunction, NULL);

    return device;
}

RTCScene initializeScene(RTCDevice device) {
    RTCScene scene = rtcNewScene(device);

    // create a triangle mesh geometry and initialize a singe triangle
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

    // vertex buffer
    float* vertices = (float*) rtcSetNewGeometryBuffer(geom,
                                                     RTC_BUFFER_TYPE_VERTEX,
                                                     0,
                                                     RTC_FORMAT_FLOAT3,
                                                     3*sizeof(float),
                                                     3);

    // index buffer
    unsigned* indices = (unsigned*) rtcSetNewGeometryBuffer(geom,
                                                          RTC_BUFFER_TYPE_INDEX,
                                                          0,
                                                          RTC_FORMAT_UINT3,
                                                          3*sizeof(unsigned),
                                                          1);
    if (vertices && indices)
    {
        vertices[0] = 0.f; vertices[1] = 0.f; vertices[2] = 0.f;
        vertices[3] = 1.f; vertices[4] = 0.f; vertices[5] = 0.f;
        vertices[6] = 0.f; vertices[7] = 1.f; vertices[8] = 0.f;

        indices[0] = 0; indices[1] = 1; indices[2] = 2;
    }

    // commit geometry objects
    rtcCommitGeometry(geom);

    // attach geometry to the scene
    rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);

    // commit scene
    rtcCommitScene(scene);

    return scene;
}

void castRay(RTCScene scene,
             float ox, float oy, float oz,
             float dx, float dy, float dz) {
    RTCRayHit rayhit;

    rayhit.ray.org_x = ox;
    rayhit.ray.org_y = oy;
    rayhit.ray.org_z = oz;
    rayhit.ray.dir_x = dx;
    rayhit.ray.dir_y = dy;
    rayhit.ray.dir_z = dz;
    rayhit.ray.tnear = 0;
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = -1;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    rtcIntersect1(scene, &rayhit);

    printf("%f, %f, %f: ", ox, oy, oz);
    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        printf("Found intersection on geometry %d, primitive %d at tfar=%f\n", 
           rayhit.hit.geomID,
           rayhit.hit.primID,
           rayhit.ray.tfar);
    } else {
        printf("Did not find any intersection.\n");
    }
}

int main() {
    std::cout << "Hello Embree!" << std::endl;

    RTCDevice device = initializeDevice();
    RTCScene scene   = initializeScene(device);

    RTCRayHit rayhit; 
    rayhit.ray.org_x  = 0.f; rayhit.ray.org_y = 0.f; rayhit.ray.org_z = -1.f;
    rayhit.ray.dir_x  = 0.f; rayhit.ray.dir_y = 0.f; rayhit.ray.dir_z =  1.f;
    rayhit.ray.tnear  = 0.f;
    rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    
    /* This will hit the triangle at t=1. */
    castRay(scene, 0.33f, 0.33f, -1, 0, 0, 1);

    /* This will not hit anything. */
    castRay(scene, 1.00f, 1.00f, -1, 0, 0, 1);

    rtcReleaseScene(scene);
    rtcReleaseDevice(device);
}