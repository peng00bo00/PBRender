#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/rng.h>

namespace PBRender
{

class Sampler {
public:
	virtual ~Sampler() {}
	virtual void StartPixelSample(Point2i p, int sampleIndex, int dimension = 0) = 0;

	virtual float Get1D() = 0;
	virtual Point2f Get2D() = 0;
	virtual Point2f GetPixel2D() = 0;

	virtual std::unique_ptr<Sampler> Clone() = 0;

	virtual std::string ToString() const = 0;
};


} // namespace PBRender