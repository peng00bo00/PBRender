#pragma once

#include "sampler.h"

namespace PBRender {

class SobolSampler : public GlobalSampler {
    public:
        // SobolSampler Public Methods
        std::unique_ptr<Sampler> Clone(int seed);

        SobolSampler(int64_t samplesPerPixel, const Bounds2i &sampleBounds)
            : GlobalSampler(RoundUpPow2(samplesPerPixel)), 
              sampleBounds(sampleBounds) {
            
            if (!IsPowerOf2(samplesPerPixel))
            // Warning("Non power-of-two sample count rounded up to %" PRId64
            //         " for SobolSampler.",
            //         this->samplesPerPixel);
                std::cerr << "Non power-of-two sample count for SobolSampler" << std::endl;
            
            resolution = RoundUpPow2(
                std::max(sampleBounds.Diagonal().x, sampleBounds.Diagonal().y));
            log2Resolution = Log2Int(resolution);
            if (resolution > 0) assert((1 << log2Resolution) == resolution);
        }

        int64_t GetIndexForSample(int64_t sampleNum) const;
        float SampleDimension(int64_t index, int dimension) const;
    
    private:
        // SobolSampler Private Data
        const Bounds2i sampleBounds;
        int resolution, log2Resolution;
};

SobolSampler *CreateSobolSampler(int nsamp, const Bounds2i &sampleBounds);

}