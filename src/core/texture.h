#pragma once

#include "PBRender.h"
#include "spectrum.h"
#include "geometry.h"
#include "transform.h"

namespace PBRender {

template <typename T>
class Texture {
    public:
		// Texture Interface
		virtual T Evaluate(const SurfaceInteraction &) const = 0;
		virtual ~Texture() {}
};

}