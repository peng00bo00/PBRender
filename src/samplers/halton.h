#pragma once

#include "sampler.h"
#include "lowdiscrepancy.h"

namespace PBRender {

class HaltonSampler : public GlobalSampler {
    public:
        HaltonSampler(int nsamp, const Bounds2i &samplBounds, bool sampleAtCenter=false);

        int64_t GetIndexForSample(int64_t sampleNum) const;
        float SampleDimension(int64_t index, int dimension) const;
        std::unique_ptr<Sampler> Clone(int seed);
    
    private:
    // HaltonSampler Private Methods
        const uint16_t *PermutationForDimension(int dim) const {
            if (dim >= PrimeTableSize)
                // LOG(FATAL) << StringPrintf("HaltonSampler can only sample %d "
                //                         "dimensions.", PrimeTableSize);
                std::cerr << "HaltonSampler can only sample " << PrimeTableSize << "dimensions" << std::endl;
            
            return &radicalInversePermutations[PrimeSums[dim]];
        }

    private:
        // HaltonSampler Private Data
        static std::vector<uint16_t> radicalInversePermutations;
        Point2i baseScales, baseExponents;
        int sampleStride;
        int multInverse[2];
        mutable Point2i pixelForOffset = Point2i(std::numeric_limits<int>::max(),
                                                 std::numeric_limits<int>::max());
        mutable int64_t offsetForCurrentPixel;
        // Added after book publication: force all image samples to be at the
        // center of the pixel area.
        bool sampleAtPixelCenter;

};

HaltonSampler *CreateHaltonSampler(int nsamp, const Bounds2i &sampleBounds, bool sampleAtCenter=false);


}