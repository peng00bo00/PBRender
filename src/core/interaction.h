#pragma once

#include "PBRender.h"
#include "geometry.h"
#include "transform.h"
#include "material.h"

namespace PBRender {

struct Interaction {
    Interaction() : time(0) {}
    Interaction(const Point3f &p, const Normal3f &n,
                const Vector3f &wo, float time)
        : p(p),
          time(time),
          wo(Normalize(wo)),
          n(n) {}

    // Interaction Public Data
    Point3f p;
    float time;
    Vector3f wo;
    Normal3f n;
};

class SurfaceInteraction : public Interaction {
    public:
		SurfaceInteraction() {}
		SurfaceInteraction(const Point3f &p, const Vector3f &pError,
						   const Point2f &uv, const Vector3f &wo,
						   const Vector3f &dpdu, const Vector3f &dpdv,
						   const Normal3f &dndu, const Normal3f &dndv, float time,
						   const Shape *sh,
						   int faceIndex = 0);
		
		~SurfaceInteraction() {
			// if(bsdf) bsdf->~BSDF();
		}
		
		void SetShadingGeometry(const Vector3f &dpdu, const Vector3f &dpdv,
                                const Normal3f &dndu, const Normal3f &dndv,
                                bool orientationIsAuthoritative);
		
		void ComputeScatteringFunctions(const Ray &ray,
									    bool allowMultipleLobes = false,
										TransportMode mode = TransportMode::Radiance);
	
	public:
		Point2f uv;
		Vector3f dpdu, dpdv;
		Normal3f dndu, dndv;
		const Shape *shape = nullptr;
		struct {
			Normal3f n;
			Vector3f dpdu, dpdv;
			Normal3f dndu, dndv;
		} shading;
		const Primitive *primitive = nullptr;
		BSDF *bsdf = nullptr;
		// BSSRDF *bssrdf = nullptr;
		mutable Vector3f dpdx, dpdy;
		mutable float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;

		int faceIndex = 0;
};

class MediumInteraction : public Interaction {

};

}