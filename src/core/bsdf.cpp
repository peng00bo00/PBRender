#include <PBRender/core/bsdf.h>

namespace PBRender
{

// BSDF Method Definitions
std::string BSDF::ToString() const {
    // return StringPrintf("[ BSDF bxdf: %s shadingFrame: %s ]", bxdf, shadingFrame);
    std::ostringstream ss;
    ss << "[ BSDF bxdf: " << bxdf->ToString() << " "
       << "shadingFrame: " << shadingFrame.ToString() << " ]";

    return ss.str();
}


} // namespace PBRender