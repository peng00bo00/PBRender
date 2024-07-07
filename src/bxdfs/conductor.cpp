#include <PBRender/bxdfs/conductor.h>

namespace PBRender
{

std::string ConductorBxDF::ToString() const {
    // return StringPrintf("[ ConductorBxDF mfDistrib: %s eta: %s k: %s ]", mfDistrib, eta,
    //                     k);
    std::ostringstream ss;
    ss << "[ ConductorBxDF mfDistrib: " << mfDistrib.ToString() << " "
       << "eta: " << eta << " "
       << "k: " << k << " ]";

    return ss.str();
}

} // namespace PBRender