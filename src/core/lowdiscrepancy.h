#pragma once

#include "PBRender.h"
#include "rng.h"
#include "sampling.h"
#include "sobolmatrices.h"

namespace PBRender {

// Low Discrepancy Declarations
float RadicalInverse(int baseIndex, uint64_t a);
std::vector<uint16_t> ComputeRadicalInversePermutations(RNG &rng);
static constexpr int PrimeTableSize = 1000;
extern const int Primes[PrimeTableSize];
float ScrambledRadicalInverse(int baseIndex, uint64_t a, const uint16_t *perm);
extern const int PrimeSums[PrimeTableSize];
inline void Sobol2D(int nSamplesPerPixelSample, int nPixelSamples,
                    Point2f *samples, RNG &rng);
extern uint32_t CMaxMinDist[17][32];
inline uint64_t SobolIntervalToIndex(const uint32_t log2Resolution,
                                     uint64_t sampleNum, const Point2i &p);
inline float SobolSampleFloat(int64_t index, int dimension,
                              uint32_t scramble = 0);
inline double SobolSampleDouble(int64_t index, int dimension,
                                uint64_t scramble = 0);

// Low Discrepancy Inline Functions
inline uint32_t ReverseBits32(uint32_t n) {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

inline uint64_t ReverseBits64(uint64_t n) {
    uint64_t n0 = ReverseBits32((uint32_t)n);
    uint64_t n1 = ReverseBits32((uint32_t)(n >> 32));
    return (n0 << 32) | n1;
}

template <int base>
inline uint64_t InverseRadicalInverse(uint64_t inverse, int nDigits) {
    uint64_t index = 0;
    for (int i = 0; i < nDigits; ++i) {
        uint64_t digit = inverse % base;
        inverse /= base;
        index = index * base + digit;
    }
    return index;
}

inline uint32_t MultiplyGenerator(const uint32_t *C, uint32_t a) {
    uint32_t v = 0;
    for (int i = 0; a != 0; ++i, a >>= 1)
        if (a & 1) v ^= C[i];
    return v;
}

inline float SampleGeneratorMatrix(const uint32_t *C, uint32_t a,
                                   uint32_t scramble = 0) {
    return std::min((MultiplyGenerator(C, a) ^ scramble) * float(2.3283064365386963e-10),
                    OneMinusEpsilon);
}

inline uint32_t GrayCode(uint32_t v) { return (v >> 1) ^ v; }

inline void GrayCodeSample(const uint32_t *C, uint32_t n, uint32_t scramble,
                           float *p) {
    uint32_t v = scramble;
    for (uint32_t i = 0; i < n; ++i) {
        p[i] = std::min(v * float(2.3283064365386963e-10) /* 1/2^32 */,
                        OneMinusEpsilon);
        v ^= C[CountTrailingZeros(i + 1)];
    }
}

inline void GrayCodeSample(const uint32_t *C0, const uint32_t *C1, uint32_t n,
                           const Point2i &scramble, Point2f *p) {
    uint32_t v[2] = {(uint32_t)scramble.x, (uint32_t)scramble.y};
    for (uint32_t i = 0; i < n; ++i) {
        p[i].x = std::min(v[0] * float(2.3283064365386963e-10), OneMinusEpsilon);
        p[i].y = std::min(v[1] * float(2.3283064365386963e-10), OneMinusEpsilon);
        v[0] ^= C0[CountTrailingZeros(i + 1)];
        v[1] ^= C1[CountTrailingZeros(i + 1)];
    }
}

inline void VanDerCorput(int nSamplesPerPixelSample, int nPixelSamples,
                         float *samples, RNG &rng) {
    uint32_t scramble = rng.UniformUInt32();
    // Define _CVanDerCorput_ Generator Matrix
    const uint32_t CVanDerCorput[32] = {
        0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x8000000, 0x4000000,
        0x2000000,  0x1000000,  0x800000,   0x400000,   0x200000,  0x100000,
        0x80000,    0x40000,    0x20000,    0x10000,    0x8000,    0x4000,
        0x2000,     0x1000,     0x800,      0x400,      0x200,     0x100,
        0x80,       0x40,       0x20,       0x10,       0x8,       0x4,
        0x2,        0x1
    };
    int totalSamples = nSamplesPerPixelSample * nPixelSamples;
    GrayCodeSample(CVanDerCorput, totalSamples, scramble, samples);
    // Randomly shuffle 1D sample points
    for (int i = 0; i < nPixelSamples; ++i)
        Shuffle(samples + i * nSamplesPerPixelSample, nSamplesPerPixelSample, 1,
                rng);
    Shuffle(samples, nPixelSamples, nSamplesPerPixelSample, rng);
}

inline void Sobol2D(int nSamplesPerPixelSample, int nPixelSamples,
                    Point2f *samples, RNG &rng) {
    Point2i scramble;
    scramble[0] = rng.UniformUInt32();
    scramble[1] = rng.UniformUInt32();

    // Define 2D Sobol$'$ generator matrices _CSobol[2]_
    const uint32_t CSobol[2][32] = {
        {0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x8000000, 0x4000000,
         0x2000000, 0x1000000, 0x800000, 0x400000, 0x200000, 0x100000, 0x80000,
         0x40000, 0x20000, 0x10000, 0x8000, 0x4000, 0x2000, 0x1000, 0x800,
         0x400, 0x200, 0x100, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1},
        {0x80000000, 0xc0000000, 0xa0000000, 0xf0000000, 0x88000000, 0xcc000000,
         0xaa000000, 0xff000000, 0x80800000, 0xc0c00000, 0xa0a00000, 0xf0f00000,
         0x88880000, 0xcccc0000, 0xaaaa0000, 0xffff0000, 0x80008000, 0xc000c000,
         0xa000a000, 0xf000f000, 0x88008800, 0xcc00cc00, 0xaa00aa00, 0xff00ff00,
         0x80808080, 0xc0c0c0c0, 0xa0a0a0a0, 0xf0f0f0f0, 0x88888888, 0xcccccccc,
         0xaaaaaaaa, 0xffffffff}};
    GrayCodeSample(CSobol[0], CSobol[1], nSamplesPerPixelSample * nPixelSamples,
                   scramble, samples);
    for (int i = 0; i < nPixelSamples; ++i)
        Shuffle(samples + i * nSamplesPerPixelSample, nSamplesPerPixelSample, 1,
                rng);
    Shuffle(samples, nPixelSamples, nSamplesPerPixelSample, rng);
}

inline uint64_t SobolIntervalToIndex(const uint32_t m, uint64_t frame,
                                     const Point2i &p) {
    if (m == 0) return 0;

    const uint32_t m2 = m << 1;
    uint64_t index = uint64_t(frame) << m2;

    uint64_t delta = 0;
    for (int c = 0; frame; frame >>= 1, ++c)
        if (frame & 1)  // Add flipped column m + c + 1.
            delta ^= VdCSobolMatrices[m - 1][c];

    // flipped b
    uint64_t b = (((uint64_t)((uint32_t)p.x) << m) | ((uint32_t)p.y)) ^ delta;

    for (int c = 0; b; b >>= 1, ++c)
        if (b & 1)  // Add column 2 * m - c.
            index ^= VdCSobolMatricesInv[m - 1][c];

    return index;
}

inline float SobolSample(int64_t index, int dimension, uint64_t scramble = 0) {

    return SobolSampleFloat(index, dimension, (uint32_t)scramble);
}

inline float SobolSampleFloat(int64_t a, int dimension, uint32_t scramble) {
    // CHECK_LT(dimension, NumSobolDimensions) <<
    //     "Integrator has consumed too many Sobol' dimensions; you "
    //     "may want to use a Sampler without a dimension limit like "
    //     "\"02sequence.\"";
    assert(dimension < NumSobolDimensions);
    uint32_t v = scramble;
    for (int i = dimension * SobolMatrixSize; a != 0; a >>= 1, i++)
        if (a & 1) v ^= SobolMatrices32[i];
    return std::min(v * 2.3283064365386963e-10f /* 1/2^32 */,
                    FloatOneMinusEpsilon);
}

inline double SobolSampleDouble(int64_t a, int dimension, uint64_t scramble) {
//   CHECK_LT(dimension, NumSobolDimensions) <<
//       "Integrator has consumed too many Sobol' dimensions; you "
//       "may want to use a Sampler without a dimension limit like "
//       "\"02sequence\".";
    assert(dimension < NumSobolDimensions);
    uint64_t result = scramble & ~ - (1LL << SobolMatrixSize);
    for (int i = dimension * SobolMatrixSize; a != 0; a >>= 1, i++)
        if (a & 1) result ^= SobolMatrices64[i];
    return std::min(result * (1.0 / (1ULL << SobolMatrixSize)),
                    DoubleOneMinusEpsilon);
}

}