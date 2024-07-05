#include <PBRender/samplers/halton.h>

namespace PBRender
{

HaltonSampler::HaltonSampler(int samplesPerPixel, Point2i fullRes,
                             RandomizeStrategy randomize, int seed)
    : samplesPerPixel(samplesPerPixel), randomize(randomize) {
    if (randomize == RandomizeStrategy::PermuteDigits)
        digitPermutations = ComputeRadicalInversePermutations(seed);
    
    // Find radical inverse base scales and exponents that cover sampling area
    for (int i = 0; i < 2; ++i) {
        int base = (i == 0) ? 2 : 3;
        int scale = 1, exp = 0;
        while (scale < std::min(fullRes[i], MaxHaltonResolution)) {
            scale *= base;
            ++exp;
        }
        baseScales[i] = scale;
        baseExponents[i] = exp;
    }

    // Compute multiplicative inverses for _baseScales_
    multInverse[0] = multiplicativeInverse(baseScales[1], baseScales[0]);
    multInverse[1] = multiplicativeInverse(baseScales[0], baseScales[1]);

}

std::string HaltonSampler::ToString() const {
    // return StringPrintf("[ HaltonSampler randomize: %s digitPermutations: %p "
    //                 "haltonIndex: %d dimension: %d samplesPerPixel: %d "
    //                 "baseScales: %s baseExponents: %s multInverse: [ %d %d ] ]",
    //                 randomize, digitPermutations, haltonIndex, dimension,
    //                 samplesPerPixel, baseScales, baseExponents, multInverse[0],
    //                 multInverse[1]);


    std::ostringstream ss;
    // ss << "[ HaltonSampler randomize: " << randomize << " "
    ss << "[ HaltonSampler "
    //    << "digitPermutations: " << digitPermutations << " "
       << "dimension: " << haltonIndex << " "
       << "samplesPerPixel: " << samplesPerPixel << " "
       << "baseScales: " << baseScales << " "
       << "baseExponents: " << baseExponents << " "
       << "multInverse: [" << multInverse[0] << ", " << multInverse[1] << "] ]";

    return ss.str();
}

std::unique_ptr<Sampler> HaltonSampler::Clone() {
    return std::make_unique<HaltonSampler>(*this);
}

} // namespace PBRender