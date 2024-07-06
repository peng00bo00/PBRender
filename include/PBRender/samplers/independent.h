#pragma once

#include <PBRender/core/sampler.h>

namespace PBRender
{

/* A simple independednt sampler. Note that this sampler should NEVER
 be used in renedering.
*/
class IndependentSampler : public Sampler {
public:
    IndependentSampler(int samplesPerPixel, int seed = 0)
        : samplesPerPixel(samplesPerPixel), seed(seed) {}
    
    ~IndependentSampler() { }
    
    void StartPixelSample(Point2i p, int sampleIndex, int dimension) {
        rng.SetSequence(Hash(p, seed));
        rng.Advance(sampleIndex * 65536ull + dimension);
    }

    int SamplesPerPixel() const { return samplesPerPixel; }

    float Get1D() { return rng.Uniform<float>(); }
    Point2f Get2D() { return {rng.Uniform<float>(), rng.Uniform<float>()}; }
    Point2f GetPixel2D() { return Get2D(); }

    std::unique_ptr<Sampler> Clone();
    std::string ToString() const;

private:
    int samplesPerPixel, seed;
    RNG rng;
};

} // namespace PBRender