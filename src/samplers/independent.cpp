#include <PBRender/samplers/independent.h>

namespace PBRender
{

std::string IndependentSampler::ToString() const {
    std::ostringstream ss;
    ss << "[ IndependentSampler samplesPerPixel: " << samplesPerPixel << " " 
       << "seed: " << seed << " "
       << "rng: " << rng.ToString() << " ]";

    return ss.str();
}

std::unique_ptr<Sampler> IndependentSampler::Clone() {
    return std::make_unique<IndependentSampler>(*this);
}

} // namespace PBRender