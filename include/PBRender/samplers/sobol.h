#pragma once

#include <PBRender/core/sampler.h>
#include <PBRender/samplers/lowdiscrepancy.h>

namespace PBRender
{

class SobolSampler : public Sampler {
public:
    // SobolSampler Public Methods
    SobolSampler(int samplesPerPixel, Point2i fullResolution, RandomizeStrategy randomize,
                 int seed = 0)
        : samplesPerPixel(samplesPerPixel), seed(seed), randomize(randomize) {
        if (!IsPowerOf2(samplesPerPixel))
            // Warning("Non power-of-two sample count %d will perform suboptimally with the "
            //         "SobolSampler.",
            //         samplesPerPixel);
            std::cout << "Non power-of-two sample count " << samplesPerPixel << " "
                      << "will perform suboptimally with the SobolSampler." << std::endl;

        scale = RoundUpPow2(std::max(fullResolution.x, fullResolution.y));
    }

    
    // static constexpr const char *Name() { return "SobolSampler"; }
    // static SobolSampler *Create(const ParameterDictionary &parameters,
    //                             Point2i fullResolution, const FileLoc *loc,
    //                             Allocator alloc);

    
    int SamplesPerPixel() const { return samplesPerPixel; }
    
    void StartPixelSample(Point2i p, int sampleIndex, int dim) {
        pixel = p;
        dimension = std::max(2, dim);
        sobolIndex = SobolIntervalToIndex(Log2Int(scale), sampleIndex, pixel);
    }

    
    float Get1D() {
        if (dimension >= NSobolDimensions)
            dimension = 2;
        return SampleDimension(dimension++);
    }

    
    Point2f Get2D() {
        if (dimension + 1 >= NSobolDimensions)
            dimension = 2;
        Point2f u(SampleDimension(dimension), SampleDimension(dimension + 1));
        dimension += 2;
        return u;
    }

    
    Point2f GetPixel2D() {
        Point2f u(SobolSample(sobolIndex, 0, NoRandomizer()),
                  SobolSample(sobolIndex, 1, NoRandomizer()));
        // Remap Sobol\+$'$ dimensions used for pixel samples
        for (int dim = 0; dim < 2; ++dim) {
            // DCHECK_RARE(1e-7, u[dim] * scale - pixel[dim] < 0);
            // DCHECK_RARE(1e-7, u[dim] * scale - pixel[dim] > 1);
            u[dim] = Clamp(u[dim] * scale - pixel[dim], 0.f, OneMinusEpsilon);
        }

        return u;
    }

    std::unique_ptr<Sampler> Clone();
    std::string ToString() const;

private:
    float SampleDimension(int dimension) const {
        // Return un-randomized Sobol\+$'$ sample if appropriate
        if (randomize == RandomizeStrategy::None)
            return SobolSample(sobolIndex, dimension, NoRandomizer());

        // Return randomized Sobol\+$'$ sample using _randomize_
        uint32_t hash = Hash(dimension, seed);
        if (randomize == RandomizeStrategy::PermuteDigits)
            return SobolSample(sobolIndex, dimension, BinaryPermuteScrambler(hash));
        else if (randomize == RandomizeStrategy::FastOwen)
            return SobolSample(sobolIndex, dimension, FastOwenScrambler(hash));
        else
            return SobolSample(sobolIndex, dimension, OwenScrambler(hash));
    }

private:
    int samplesPerPixel, scale, seed;
    RandomizeStrategy randomize;
    Point2i pixel;
    int dimension;
    int64_t sobolIndex;
};


class PaddedSobolSampler : public Sampler {
public:
    // static constexpr const char *Name() { return "PaddedSobolSampler"; }
    // static PaddedSobolSampler *Create(const ParameterDictionary &parameters,
    //                                   const FileLoc *loc, Allocator alloc);

    PaddedSobolSampler(int samplesPerPixel, RandomizeStrategy randomizer, int seed = 0)
        : samplesPerPixel(samplesPerPixel), randomize(randomizer), seed(seed) {
        if (!IsPowerOf2(samplesPerPixel))
        //     Warning(
        //         "Sobol samplers with non power-of-two sample counts (%d) are suboptimal.",
        //         samplesPerPixel);
            std::cout << "Sobol samplers with non power-of-two sample counts " << samplesPerPixel << " "
                      << "are suboptimal." << std::endl;
    }

    int SamplesPerPixel() const { return samplesPerPixel; }

    void StartPixelSample(Point2i p, int index, int dim) {
        pixel = p;
        sampleIndex = index;
        dimension = dim;
    }
    
    float Get1D() {
        // Get permuted index for current pixel sample
        uint64_t hash = Hash(pixel, dimension, seed);
        int index = PermutationElement(sampleIndex, samplesPerPixel, hash);

        int dim = dimension++;
        // Return randomized 1D van der Corput sample for dimension _dim_
        return SampleDimension(0, index, hash >> 32);
    }
    
    Point2f Get2D() {
        // Get permuted index for current pixel sample
        uint64_t hash = Hash(pixel, dimension, seed);
        int index = PermutationElement(sampleIndex, samplesPerPixel, hash);

        int dim = dimension;
        dimension += 2;
        // Return randomized 2D Sobol\+$'$ sample
        return Point2f(SampleDimension(0, index, uint32_t(hash)),
                       SampleDimension(1, index, hash >> 32));
    }
    
    Point2f GetPixel2D() { return Get2D(); }
    
    RandomizeStrategy GetRandomizeStrategy() const { return randomize; }

    std::unique_ptr<Sampler> Clone();
    std::string ToString() const;

private:
    float SampleDimension(int dimension, uint32_t a, uint32_t hash) const {
        if (randomize == RandomizeStrategy::None)
            return SobolSample(a, dimension, NoRandomizer());
        else if (randomize == RandomizeStrategy::PermuteDigits)
            return SobolSample(a, dimension, BinaryPermuteScrambler(hash));
        else if (randomize == RandomizeStrategy::FastOwen)
            return SobolSample(a, dimension, FastOwenScrambler(hash));
        else
            return SobolSample(a, dimension, OwenScrambler(hash));
    }

private:
    // PaddedSobolSampler Private Members
    int samplesPerPixel, seed;
    RandomizeStrategy randomize;
    Point2i pixel;
    int sampleIndex, dimension;
};

// ZSobolSampler. This is the default sampler in PBRT
class ZSobolSampler : public Sampler {
public:
    ZSobolSampler(int samplesPerPixel, Point2i fullResolution,
                  RandomizeStrategy randomize, int seed = 0)
        : randomize(randomize), seed(seed) {
        if (!IsPowerOf2(samplesPerPixel))
            // Warning(
            //     "Sobol samplers with non power-of-two sample counts (%d) are suboptimal.",
            //     samplesPerPixel);
            std::cout << "Sobol samplers with non power-of-two sample counts " << samplesPerPixel << " "
                      << "are suboptimal." << std::endl;
        
        log2SamplesPerPixel = Log2Int(samplesPerPixel);
        int res = RoundUpPow2(std::max(fullResolution.x, fullResolution.y));
        int log4SamplesPerPixel = (log2SamplesPerPixel + 1) / 2;
        nBase4Digits = Log2Int(res) + log4SamplesPerPixel;
    }

    
    // static constexpr const char *Name() { return "ZSobolSampler"; }

    // static ZSobolSampler *Create(const ParameterDictionary &parameters,
    //                              Point2i fullResolution, const FileLoc *loc,
    //                              Allocator alloc);

    
    int SamplesPerPixel() const { return 1 << log2SamplesPerPixel; }

    
    void StartPixelSample(Point2i p, int index, int dim) {
        dimension = dim;
        mortonIndex = (EncodeMorton2(p.x, p.y) << log2SamplesPerPixel) | index;
    }

    
    float Get1D() {
        uint64_t sampleIndex = GetSampleIndex();
        ++dimension;
        // Generate 1D Sobol\+$'$ sample at _sampleIndex_
        uint32_t sampleHash = Hash(dimension, seed);
        if (randomize == RandomizeStrategy::None)
            return SobolSample(sampleIndex, 0, NoRandomizer());
        else if (randomize == RandomizeStrategy::PermuteDigits)
            return SobolSample(sampleIndex, 0, BinaryPermuteScrambler(sampleHash));
        else if (randomize == RandomizeStrategy::FastOwen)
            return SobolSample(sampleIndex, 0, FastOwenScrambler(sampleHash));
        else
            return SobolSample(sampleIndex, 0, OwenScrambler(sampleHash));
    }

    
    Point2f Get2D() {
        uint64_t sampleIndex = GetSampleIndex();
        dimension += 2;
        // Generate 2D Sobol\+$'$ sample at _sampleIndex_
        uint64_t bits = Hash(dimension, seed);
        uint32_t sampleHash[2] = {uint32_t(bits), uint32_t(bits >> 32)};
        if (randomize == RandomizeStrategy::None)
            return {SobolSample(sampleIndex, 0, NoRandomizer()),
                    SobolSample(sampleIndex, 1, NoRandomizer())};
        else if (randomize == RandomizeStrategy::PermuteDigits)
            return {SobolSample(sampleIndex, 0, BinaryPermuteScrambler(sampleHash[0])),
                    SobolSample(sampleIndex, 1, BinaryPermuteScrambler(sampleHash[1]))};
        else if (randomize == RandomizeStrategy::FastOwen)
            return {SobolSample(sampleIndex, 0, FastOwenScrambler(sampleHash[0])),
                    SobolSample(sampleIndex, 1, FastOwenScrambler(sampleHash[1]))};
        else
            return {SobolSample(sampleIndex, 0, OwenScrambler(sampleHash[0])),
                    SobolSample(sampleIndex, 1, OwenScrambler(sampleHash[1]))};
    }
    
    Point2f GetPixel2D() { return Get2D(); }

    std::unique_ptr<Sampler> Clone();
    std::string ToString() const;

    uint64_t GetSampleIndex() const {
        // Define the full set of 4-way permutations in _permutations_
        static const uint8_t permutations[24][4] = {
            {0, 1, 2, 3},
            {0, 1, 3, 2},
            {0, 2, 1, 3},
            {0, 2, 3, 1},
            // Define remaining 20 4-way permutations
            {0, 3, 2, 1},
            {0, 3, 1, 2},
            {1, 0, 2, 3},
            {1, 0, 3, 2},
            {1, 2, 0, 3},
            {1, 2, 3, 0},
            {1, 3, 2, 0},
            {1, 3, 0, 2},
            {2, 1, 0, 3},
            {2, 1, 3, 0},
            {2, 0, 1, 3},
            {2, 0, 3, 1},
            {2, 3, 0, 1},
            {2, 3, 1, 0},
            {3, 1, 2, 0},
            {3, 1, 0, 2},
            {3, 2, 1, 0},
            {3, 2, 0, 1},
            {3, 0, 2, 1},
            {3, 0, 1, 2}

        };

        uint64_t sampleIndex = 0;
        // Apply random permutations to full base-4 digits
        bool pow2Samples = log2SamplesPerPixel & 1;
        int lastDigit = pow2Samples ? 1 : 0;
        for (int i = nBase4Digits - 1; i >= lastDigit; --i) {
            // Randomly permute $i$th base-4 digit in _mortonIndex_
            int digitShift = 2 * i - (pow2Samples ? 1 : 0);
            int digit = (mortonIndex >> digitShift) & 3;
            // Choose permutation _p_ to use for _digit_
            uint64_t higherDigits = mortonIndex >> (digitShift + 2);
            int p = (MixBits(higherDigits ^ (0x55555555u * dimension)) >> 24) % 24;

            digit = permutations[p][digit];
            sampleIndex |= uint64_t(digit) << digitShift;
        }

        // Handle power-of-2 (but not 4) sample count
        if (pow2Samples) {
            int digit = mortonIndex & 1;
            sampleIndex |=
                digit ^ (MixBits((mortonIndex >> 1) ^ (0x55555555u * dimension)) & 1);
        }

        return sampleIndex;
    }

private:
    RandomizeStrategy randomize;
    int seed, log2SamplesPerPixel, nBase4Digits;
    uint64_t mortonIndex;
    int dimension;
};

} // namespace PBRender