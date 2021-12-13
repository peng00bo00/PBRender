#pragma once

#include "PBRender.h"
#include "geometry.h"
#include "transform.h"

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

};

}