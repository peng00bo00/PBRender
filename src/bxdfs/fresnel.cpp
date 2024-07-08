#include <PBRender/bxdfs/fresnel.h>

namespace PBRender
{

// std::string DiffuseBxDF::ToString() const {
//     // return StringPrintf("[ DiffuseBxDF R: %s ]", R);
//     std::ostringstream ss;
//     ss << "[ DiffuseBxDF R: " << R << " ]";

//     return ss.str();
// }

std::string NormalizedFresnelBxDF::ToString() const {
    // return StringPrintf("[ NormalizedFresnelBxDF eta: %f ]", eta);
    std::ostringstream ss;
    ss << "[ NormalizedFresnelBxDF eta: " << eta << " ]";

    return ss.str();
}

} // namespace PBRender