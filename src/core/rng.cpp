#include <PBRender/core/rng.h>

namespace PBRender
{

std::string RNG::ToString() const {
    // return StringPrintf("[ RNG state: %" PRIu64 " inc: %" PRIu64 " ]", state, inc);

    std::ostringstream ss;
    ss << "[ RNG state: " << state << " " << "inc: " << inc << " ]";

    return ss.str();
}

} // namespace PBRender