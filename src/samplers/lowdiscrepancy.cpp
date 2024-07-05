#include <PBRender/samplers/lowdiscrepancy.h>

namespace PBRender
{

std::string DigitPermutation::ToString() const {
    // std::string s = StringPrintf(
    //     "[ DigitPermitation base: %d nDigits: %d permutations: ", base, nDigits);
    // for (int digitIndex = 0; digitIndex < nDigits; ++digitIndex) {
    //     s += StringPrintf("[%d] ( ", digitIndex);
    //     for (int digitValue = 0; digitValue < base; ++digitValue) {
    //         s += StringPrintf("%d", permutations[digitIndex * base + digitValue]);
    //         if (digitValue != base - 1)
    //             s += ", ";
    //     }
    //     s += ") ";
    // }

    std::ostringstream ss;
    ss << "[ DigitPermitation base: " << base << " nDigits: " << nDigits << "permutations: ";
    for (int digitIndex = 0; digitIndex < nDigits; ++digitIndex) {
        // s += StringPrintf("[%d] ( ", digitIndex);
        ss << "[" << digitIndex << "] (";
        for (int digitValue = 0; digitValue < base; ++digitValue) {
            // s += StringPrintf("%d", permutations[digitIndex * base + digitValue]);
            ss << permutations[digitIndex * base + digitValue];
            if (digitValue != base - 1)
                ss << ", ";
        }
        ss << ") ";
    }

    ss << " ]";

    return ss.str();
}

std::string ToString(RandomizeStrategy r) {
    switch (r) {
    case RandomizeStrategy::None:
        return "None";
    case RandomizeStrategy::PermuteDigits:
        return "PermuteDigits";
    case RandomizeStrategy::FastOwen:
        return "FastOwen";
    case RandomizeStrategy::Owen:
        return "Owen";
    default:
        // LOG_FATAL("Unhandled RandomizeStrategy");
        std::cout << "Unhandled RandomizeStrategy" << std::endl;
        return "";
    }
}

// Low Discrepancy Function Definitions
std::vector<DigitPermutation> ComputeRadicalInversePermutations(uint32_t seed) {
    std::vector<DigitPermutation> perms;
    perms.resize(PrimeTableSize);
    for (int i = 0; i < PrimeTableSize; ++i)
        perms[i] = DigitPermutation(Primes[i], seed);
    return perms;
}


} // namespace PBRender