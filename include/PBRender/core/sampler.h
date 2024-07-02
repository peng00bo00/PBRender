#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/rng.h>

namespace PBRender
{

class Sampler {
public:
    virtual ~Sampler();
    Sampler(int64_t samplesPerPixel);

    virtual void StartPixel(const Point2i &p);

    virtual float Get1D() = 0;
    virtual Point2f Get2D() = 0;

    CameraSample GetCameraSample(const Point2i &pRaster);

    void Request1DArray(int n);
    void Request2DArray(int n);

    virtual int RoundCount(int n) const { return n; }

    const float *Get1DArray(int n);
    const Point2f *Get2DArray(int n);

    virtual bool StartNextSample();
    virtual std::unique_ptr<Sampler> Clone(int seed) = 0;
    virtual bool SetSampleNumber(int64_t sampleNum);
    // std::string StateString() const {
    //   return StringPrintf("(%d,%d), sample %" PRId64, currentPixel.x,
    //                       currentPixel.y, currentPixelSampleIndex);
    // }

    const int64_t samplesPerPixel;

protected:
    Point2i currentPixel;
    int64_t currentPixelSampleIndex;
    std::vector<int> samples1DArraySizes, samples2DArraySizes;
    std::vector<std::vector<float>> sampleArray1D;
    std::vector<std::vector<Point2f>> sampleArray2D;

private:
    size_t array1DOffset, array2DOffset;
};

class PixelSampler : public Sampler {
  public:
    PixelSampler(int64_t samplesPerPixel, int nSampledDimensions);
    bool StartNextSample();
    bool SetSampleNumber(int64_t);
    float Get1D();
    Point2f Get2D();

  protected:
    std::vector<std::vector<float>> samples1D;
    std::vector<std::vector<Point2f>> samples2D;
    int current1DDimension = 0, current2DDimension = 0;
    RNG rng;
};

class GlobalSampler : public Sampler {
  public:
    // GlobalSampler Public Methods
    bool StartNextSample();
    void StartPixel(const Point2i &);
    bool SetSampleNumber(int64_t sampleNum);
    float Get1D();
    Point2f Get2D();
    GlobalSampler(int64_t samplesPerPixel) : Sampler(samplesPerPixel) {}
    virtual int64_t GetIndexForSample(int64_t sampleNum) const = 0;
    virtual float SampleDimension(int64_t index, int dimension) const = 0;

  private:
    int dimension;
    int64_t intervalSampleIndex;
    static const int arrayStartDim = 5;
    int arrayEndDim;
};


} // namespace PBRender