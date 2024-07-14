#include <PBRender/textures/constant.h>

namespace PBRender
{

std::string ConstantTexture::ToString() const {
    std::ostringstream ss;
    ss << "[ ConstantTexture value: " << value << " ]";

    return ss.str();
}



} // namespace PBRender