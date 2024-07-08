#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/bxdf.h>
#include <PBRender/core/vecmath.h>

namespace PBRender
{

class BSDF {
public:
    // BSDF Public Methods
    BSDF() = default;
    BSDF(Normal3f ns, Vector3f dpdus, BxDF *bxdf)
        : bxdf(bxdf), shadingFrame(Frame::FromXZ(Normalize(dpdus), Vector3f(ns))) {}

    operator bool() const { return (bool)bxdf; }
    
    BxDFFlags Flags() const { return bxdf->Flags(); }
    
    Vector3f RenderToLocal(Vector3f v) const { return shadingFrame.ToLocal(v); }
    Vector3f LocalToRender(Vector3f v) const { return shadingFrame.FromLocal(v); }

    Spectrum f(Vector3f woRender, Vector3f wiRender,
                      TransportMode mode = TransportMode::Radiance) const {
        Vector3f wi = RenderToLocal(wiRender), wo = RenderToLocal(woRender);
        if (wo.z == 0)
            return {};
        return bxdf->f(wo, wi, mode);
    }

    template <typename BxDF>
    Spectrum f(Vector3f woRender, Vector3f wiRender,
                                   TransportMode mode = TransportMode::Radiance) const {
        Vector3f wi = RenderToLocal(wiRender), wo = RenderToLocal(woRender);
        if (wo.z == 0)
            return {};
        const BxDF *specificBxDF = bxdf->CastOrNullptr<BxDF>();
        return specificBxDF->f(wo, wi, mode);
    }

    
    BSDFSample Sample_f(
        Vector3f woRender, float u, Point2f u2,
        TransportMode mode = TransportMode::Radiance,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
        Vector3f wo = RenderToLocal(woRender);
        if (wo.z == 0 || !(bxdf->Flags() & sampleFlags))
            return {};
        // Sample _bxdf_ and return _BSDFSample_
        BSDFSample bs = bxdf->Sample_f(wo, u, u2, mode, sampleFlags);
        if (bs)
            DCHECK_GE(bs->pdf, 0);
        if (!bs || !bs->f || bs->pdf == 0 || bs->wi.z == 0)
            return {};
        PBRT_DBG("For wo = (%f, %f, %f), ns %f %f %f sampled f = %f %f %f %f, pdf = %f, "
                 "ratio[0] = %f wi = (%f, %f, %f)\n",
                 wo.x, wo.y, wo.z, shadingFrame.z.x, shadingFrame.z.y, shadingFrame.z.z,
                 bs->f[0], bs->f[1], bs->f[2], bs->f[3], bs->pdf,
                 (bs->pdf > 0) ? (bs->f[0] / bs->pdf) : 0, bs->wi.x, bs->wi.y, bs->wi.z);
        bs->wi = LocalToRender(bs->wi);
        return bs;
    }

    
    float PDF(Vector3f woRender, Vector3f wiRender,
              TransportMode mode = TransportMode::Radiance,
              BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
        Vector3f wo = RenderToLocal(woRender), wi = RenderToLocal(wiRender);
        if (wo.z == 0)
            return 0;
        return bxdf->PDF(wo, wi, mode, sampleFlags);
    }

    template <typename BxDF>
    BSDFSample Sample_f(
        Vector3f woRender, float u, Point2f u2,
        TransportMode mode = TransportMode::Radiance,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
        Vector3f wo = RenderToLocal(woRender);
        if (wo.z == 0)
            return {};

        const BxDF *specificBxDF = bxdf->Cast<BxDF>();
        if (!(specificBxDF->Flags() & sampleFlags))
            return {};

        pstd::optional<BSDFSample> bs =
            specificBxDF->Sample_f(wo, u, u2, mode, sampleFlags);
        if (!bs || !bs->f || bs->pdf == 0 || bs->wi.z == 0)
            return {};
        // DCHECK_GT(bs->pdf, 0);

        // PBRT_DBG("For wo = (%f, %f, %f) ns %f %f %f sampled f = %f %f %f %f, pdf = %f, "
        //          "ratio[0] = %f wi = (%f, %f, %f)\n",
        //          wo.x, wo.y, wo.z, shadingFrame.z.x, shadingFrame.z.y, shadingFrame.z.z,
        //          bs->f[0], bs->f[1], bs->f[2], bs->f[3], bs->pdf,
        //          (bs->pdf > 0) ? (bs->f[0] / bs->pdf) : 0, bs->wi.x, bs->wi.y, bs->wi.z);

        bs->wi = LocalToRender(bs->wi);

        return bs;
    }

    template <typename BxDF>
    float PDF(Vector3f woRender, Vector3f wiRender,
        TransportMode mode = TransportMode::Radiance,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
        Vector3f wo = RenderToLocal(woRender), wi = RenderToLocal(wiRender);
        if (wo.z == 0)
            return 0;
        const BxDF *specificBxDF = bxdf->Cast<BxDF>();
        return specificBxDF->PDF(wo, wi, mode, sampleFlags);
    }

    std::string ToString() const;
    
    Spectrum rho(std::vector<Point2f> &u1, std::vector<float> &uc, std::vector<Point2f> &u2) const {
        return bxdf->rho(u1, uc, u2);
    }
    
    Spectrum rho(Vector3f woRender, std::vector<float> uc, std::vector<Point2f> u) const {
        Vector3f wo = RenderToLocal(woRender);
        return bxdf->rho(wo, uc, u);
    }

    void Regularize() { bxdf->Regularize(); }

private:
    // BSDF Private Members
    // TODO: maybe update bxdf to std::shared_ptr<BxDF>
    BxDF *bxdf;
    Frame shadingFrame;
};


} // namespace PBRender