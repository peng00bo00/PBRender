#include <PBRender/bxdfs/diffuse.h>

namespace PBRender
{

std::string DiffuseBxDF::ToString() const {
    // return StringPrintf("[ DiffuseBxDF R: %s ]", R);
    std::ostringstream ss;
    ss << "[ DiffuseBxDF R: " << R << " ]";

    return ss.str();
}

std::string DiffuseTransmissionBxDF::ToString() const {
    // return StringPrintf("[ DiffuseTransmissionBxDF R: %s T: %s ]", R, T);
    std::ostringstream ss;
    ss << "[ DiffuseTransmissionBxDF R: " << R << " "
       << "T: " << T << " ]";

    return ss.str();
}

} // namespace PBRender