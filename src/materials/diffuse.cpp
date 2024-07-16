#include <PBRender/materials/diffuse.h>

namespace PBRender
{

std::string DiffuseMaterial::ToString() const {
    std::ostringstream ss;
    ss << "[ DiffuseMaterial reflectance: " << reflectance->ToString() << " ]";

    return ss.str();
}

std::string DiffuseTransmissionMaterial::ToString() const {
    std::ostringstream ss;
    ss << "[ DiffuseTransmissionMaterial reflectance: " << reflectance->ToString() << " "
       << "transmittance: " << transmittance->ToString() << " ]";

    return ss.str();
}


} // namespace PBRender