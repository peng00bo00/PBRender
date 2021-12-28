#include "lightdistrib.h"
#include "lowdiscrepancy.h"
// #include "parallel.h"
#include "scene.h"
// #include "stats.h"
#include "integrator.h"

namespace PBRender {

LightDistribution::~LightDistribution() {}

std::unique_ptr<LightDistribution> CreateLightSampleDistribution(
    const std::string &name, const Scene &scene) {
    // if (name == "uniform" || scene.lights.size() == 1)
    //     return std::unique_ptr<LightDistribution>{
    //         new UniformLightDistribution(scene)};
    // else if (name == "power")
    //     return std::unique_ptr<LightDistribution>{
    //         new PowerLightDistribution(scene)};
    // else if (name == "spatial")
    //     return std::unique_ptr<LightDistribution>{
    //         new SpatialLightDistribution(scene)};
    // else {
    //     // Error(
    //     //     "Light sample distribution type \"%s\" unknown. Using \"spatial\".",
    //     //     name.c_str());
    //     std::cerr << "Light sample distribution type " << name << " unknown. Using \"spatial\"."
    //               << std::endl;
    //     return std::unique_ptr<LightDistribution>{
    //         new SpatialLightDistribution(scene)};
    // }
    return std::unique_ptr<LightDistribution>{
            new UniformLightDistribution(scene)};
}

UniformLightDistribution::UniformLightDistribution(const Scene &scene) {
    std::vector<float> prob(scene.lights.size(), float(1));
    distrib.reset(new Distribution1D(&prob[0], int(prob.size())));
}

const Distribution1D *UniformLightDistribution::Lookup(const Point3f &p) const {
    return distrib.get();
}

// PowerLightDistribution::PowerLightDistribution(const Scene &scene)
//     : distrib(ComputeLightPowerDistribution(scene)) {}

// const Distribution1D *PowerLightDistribution::Lookup(const Point3f &p) const {
//     return distrib.get();
// }

}