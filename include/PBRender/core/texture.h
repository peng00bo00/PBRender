#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/spectrum.h>
#include <PBRender/core/vecmath.h>

namespace PBRender
{

struct TextureEvalContext {
    // TextureEvalContext Public Methods
    // TODO: update constructor later
    TextureEvalContext() = default;
    // TextureEvalContext(const Interaction &intr) : p(intr.p()), uv(intr.uv) {}
    // TextureEvalContext(const SurfaceInteraction &si)
    //     : p(si.p()),
    //       dpdx(si.dpdx),
    //       dpdy(si.dpdy),
    //       n(si.n),
    //       uv(si.uv),
    //       dudx(si.dudx),
    //       dudy(si.dudy),
    //       dvdx(si.dvdx),
    //       dvdy(si.dvdy),
    //       faceIndex(si.faceIndex) {}
    // TextureEvalContext(Point3f p, Vector3f dpdx, Vector3f dpdy, Normal3f n, Point2f uv,
    //                    float dudx, float dudy, float dvdx, float dvdy, int faceIndex)
    //     : p(p),
    //       dpdx(dpdx),
    //       dpdy(dpdy),
    //       n(n),
    //       uv(uv),
    //       dudx(dudx),
    //       dudy(dudy),
    //       dvdx(dvdx),
    //       dvdy(dvdy),
    //       faceIndex(faceIndex) {}

    std::string ToString() {
        std::ostringstream ss;
        ss << "[ TextureEvalContext p: " << p << " "
           << "n: " << n << " "
           << "uv: " << uv << " "
           << "faceIndex:" << faceIndex << " ]";

        return ss.str();
    }

    Point3f p;
    // Vector3f dpdx, dpdy;
    Normal3f n;
    Point2f uv;
    // float dudx = 0, dudy = 0, dvdx = 0, dvdy = 0;
    int faceIndex = 0;
};

class ConstantTexture;
// class ReflectanceConstantTexture;
class SpectrumBilerpTexture;
class SpectrumCheckerboardTexture;
class SpectrumImageTexture;
class GPUSpectrumImageTexture;
class MarbleTexture;
class SpectrumMixTexture;
class SpectrumDirectionMixTexture;
class SpectrumDotsTexture;
class SpectrumPtexTexture;
class GPUSpectrumPtexTexture;
class SpectrumScaledTexture;


class Texture {
public:
    virtual std::string ToString() const = 0;
    virtual Spectrum Evaluate(TextureEvalContext ctx) const = 0;
};

} // namespace PBRender