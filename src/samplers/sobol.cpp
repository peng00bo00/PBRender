#include <PBRender/samplers/sobol.h>

namespace PBRender
{

// SobolSampler
std::string SobolSampler::ToString() const {
    std::ostringstream ss;
    ss << "[ SobolSampler pixel: " << pixel << " "
       << "dimension: " << dimension << " "
       << "samplesPerPixel: " << samplesPerPixel << " "
       << "seed: " << seed << " "
       << "randomize" << randomize << " ]";

    return ss.str();
}

std::unique_ptr<Sampler> SobolSampler::Clone() {
    return std::make_unique<SobolSampler>(*this);
}

// PaddedSobolSampler
std::string PaddedSobolSampler::ToString() const {
    std::ostringstream ss;
    ss << "[ PaddedSobolSampler pixel: " << pixel << " "
       << "sampleIndex: " << sampleIndex << " "
       << "samplesPerPixel: " << samplesPerPixel << " "
       << "seed: " << seed << " "
       << "randomize" << randomize << " ]";

    return ss.str();
}

std::unique_ptr<Sampler> PaddedSobolSampler::Clone() {
    return std::make_unique<PaddedSobolSampler>(*this);
}

// ZSobolSampler
std::string ZSobolSampler::ToString() const {
    std::ostringstream ss;
    ss << "[ ZSobolSampler randomize: " << randomize << " "
       << "log2SamplesPerPixel: " << log2SamplesPerPixel << " "
       << "seed: " << seed << " "
       << "nBase4Digits: " << nBase4Digits << " "
       << "mortonIndex" << mortonIndex << " "
       << "dimension" << dimension << " ]";

    return ss.str();
}

std::unique_ptr<Sampler> ZSobolSampler::Clone() {
    return std::make_unique<ZSobolSampler>(*this);
}

} // namespace PBRender