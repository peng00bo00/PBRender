#pragma once

#include "PBRender.h"

namespace PBRender {

static const double DoubleOneMinusEpsilon = 0.99999999999999989;
static const float FloatOneMinusEpsilon = 0.99999994;

static const float OneMinusEpsilon = FloatOneMinusEpsilon;

#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL

class RNG {
    public:
        // RNG Public Methods
        RNG();
        RNG(uint64_t sequenceIndex) { SetSequence(sequenceIndex); }

        void SetSequence(uint64_t sequenceIndex);

        uint32_t UniformUInt32();

        uint32_t UniformUInt32(uint32_t b) {
            uint32_t threshold = (~b + 1u) % b;

            while (true) {
                uint32_t r = UniformUInt32();
                if (r >= threshold) return r % b;
            }
        };

        float UniformFloat() {
            return std::min(OneMinusEpsilon,
                        float(UniformUInt32() * 2.3283064365386963e-10f));
        };

        template <typename Iterator>
        void Shuffle(Iterator begin, Iterator end) {
            for (Iterator it = end - 1; it > begin; --it)
                std::iter_swap(it,
                            begin + UniformUInt32((uint32_t)(it - begin + 1)));
        }

        void Advance(int64_t idelta) {
            uint64_t cur_mult = PCG32_MULT, cur_plus = inc, acc_mult = 1u,
                    acc_plus = 0u, delta = (uint64_t)idelta;
            while (delta > 0) {
                if (delta & 1) {
                    acc_mult *= cur_mult;
                    acc_plus = acc_plus * cur_mult + cur_plus;
                }
                cur_plus = (cur_mult + 1) * cur_plus;
                cur_mult *= cur_mult;
                delta /= 2;
            }
            state = acc_mult * state + acc_plus;
        }

        int64_t operator-(const RNG &other) const {
            assert(inc == other.inc);
            uint64_t cur_mult = PCG32_MULT, cur_plus = inc, cur_state = other.state,
                    the_bit = 1u, distance = 0u;
            while (state != cur_state) {
                if ((state & the_bit) != (cur_state & the_bit)) {
                    cur_state = cur_state * cur_mult + cur_plus;
                    distance |= the_bit;
                }
                assert((state & the_bit) == (cur_state & the_bit));
                the_bit <<= 1;
                cur_plus = (cur_mult + 1ULL) * cur_plus;
                cur_mult *= cur_mult;
            }
            return (int64_t)distance;
        }
    
    private:
        // RNG Private Data
        uint64_t state, inc;
};

// RNG Inline Method Definitions
inline RNG::RNG() : state(PCG32_DEFAULT_STATE), inc(PCG32_DEFAULT_STREAM) {}

inline void RNG::SetSequence(uint64_t initseq) {
    state = 0u;
    inc = (initseq << 1u) | 1u;
    UniformUInt32();
    state += PCG32_DEFAULT_STATE;
    UniformUInt32();
}

inline uint32_t RNG::UniformUInt32() {
    uint64_t oldstate = state;
    state = oldstate * PCG32_MULT + inc;
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = (uint32_t)(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}

}