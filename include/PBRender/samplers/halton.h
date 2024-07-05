#pragma once

#include <PBRender/core/sampler.h>
#include <PBRender/samplers/lowdiscrepancy.h>

namespace PBRender
{

class HaltonSampler : public Sampler {
public:
    // HaltonSampler Public Methods
    HaltonSampler(int samplesPerPixel, Point2i fullResolution,
                    RandomizeStrategy randomize = RandomizeStrategy::PermuteDigits,
                    int seed = 0);

    // static constexpr const char *Name() { return "HaltonSampler"; }
    // static HaltonSampler *Create(const ParameterDictionary &parameters,
    //                              Point2i fullResolution, const FileLoc *loc,
    //                              Allocator alloc);


    int SamplesPerPixel() const { return samplesPerPixel; }

    RandomizeStrategy GetRandomizeStrategy() const { return randomize; }

    void StartPixelSample(Point2i p, int sampleIndex, int dim) {
        haltonIndex = 0;
        int sampleStride = baseScales[0] * baseScales[1];
        // Compute Halton sample index for first sample in pixel _p_
        if (sampleStride > 1) {
            Point2i pm(Mod(p[0], MaxHaltonResolution), Mod(p[1], MaxHaltonResolution));
            for (int i = 0; i < 2; ++i) {
                uint64_t dimOffset =
                    (i == 0) ? InverseRadicalInverse(pm[i], 2, baseExponents[i])
                                : InverseRadicalInverse(pm[i], 3, baseExponents[i]);
                haltonIndex +=
                    dimOffset * (sampleStride / baseScales[i]) * multInverse[i];
            }
            haltonIndex %= sampleStride;
        }

        haltonIndex += sampleIndex * sampleStride;
        dimension = std::max(2, dim);
    }


    float Get1D() {
        if (dimension >= PrimeTableSize)
            dimension = 2;
        return SampleDimension(dimension++);
    }


    Point2f Get2D() {
        if (dimension + 1 >= PrimeTableSize)
            dimension = 2;
        int dim = dimension;
        dimension += 2;
        return {SampleDimension(dim), SampleDimension(dim + 1)};
    }


    Point2f GetPixel2D() {
        return {RadicalInverse(0, haltonIndex >> baseExponents[0]),
                RadicalInverse(1, haltonIndex / baseScales[1])};
    }

    std::unique_ptr<Sampler> Clone();
    std::string ToString() const;

private:
    // HaltonSampler Private Methods
    static uint64_t multiplicativeInverse(int64_t a, int64_t n) {
        int64_t x, y;
        extendedGCD(a, n, &x, &y);
        return Mod(x, n);
    }

    static void extendedGCD(uint64_t a, uint64_t b, int64_t *x, int64_t *y) {
        if (b == 0) {
            *x = 1;
            *y = 0;
            return;
        }
        int64_t d = a / b, xp, yp;
        extendedGCD(b, a % b, &xp, &yp);
        *x = yp;
        *y = xp - (d * yp);
    }


    float SampleDimension(int dimension) const {
        if (randomize == RandomizeStrategy::None)
            return RadicalInverse(dimension, haltonIndex);
        else if (randomize == RandomizeStrategy::PermuteDigits)
            return ScrambledRadicalInverse(dimension, haltonIndex,
                                            digitPermutations[dimension]);
        else {
            // DCHECK_EQ(randomize, RandomizeStrategy::Owen);
            return OwenScrambledRadicalInverse(dimension, haltonIndex,
                                                MixBits(1 + (dimension << 4)));
        }
    }

    // HaltonSampler Private Members
    int samplesPerPixel;
    RandomizeStrategy randomize;
    std::vector<DigitPermutation> digitPermutations;
    static constexpr int MaxHaltonResolution = 128;
    Point2i baseScales, baseExponents;
    int multInverse[2];
    int64_t haltonIndex = 0;
    int dimension = 0;
};

} // namespace PBRender