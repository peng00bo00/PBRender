#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/math.h>
#include <PBRender/core/vecmath.h>


namespace PBRender
{

inline void XYZToRGB(const float xyz[3], float rgb[3]) {
    rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
    rgb[1] =-0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
    rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ(const float rgb[3], float xyz[3]) {
    xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
    xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
    xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

enum class SpectrumType { Reflectance, Illuminant };

template <int nSpectrumSamples>
class CoefficientSpectrum {
public:
    // constructors
    CoefficientSpectrum(float v = 0.f) {
        for (int i = 0; i < nSpectrumSamples; ++i) c[i] = v;
        assert(!HasNaNs());
    }

    CoefficientSpectrum(const CoefficientSpectrum &s) {
        assert(!s.HasNaNs());
        for (int i = 0; i < nSpectrumSamples; ++i) c[i] = s.c[i];
    }

    // operators
    CoefficientSpectrum &operator+=(const CoefficientSpectrum &s2) {
        assert(!s2.HasNaNs());
        for (int i = 0; i < nSpectrumSamples; ++i) c[i] += s2.c[i];
        return *this;
    }

    CoefficientSpectrum operator+(const CoefficientSpectrum &s2) const {
        assert(!s2.HasNaNs());
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] += s2.c[i];
        return ret;
    }

    CoefficientSpectrum operator-(const CoefficientSpectrum &s2) const {
        assert(!s2.HasNaNs());
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] -= s2.c[i];
        return ret;
    }

    CoefficientSpectrum operator/(const CoefficientSpectrum &s2) const {
        assert(!s2.HasNaNs());
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; ++i) {
            assert(s2.c[i] != 0);
            ret.c[i] /= s2.c[i];
        }
        return ret;
    }

    CoefficientSpectrum operator*(const CoefficientSpectrum &sp) const {
        assert(!sp.HasNaNs());
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] *= sp.c[i];
        return ret;
    }

    CoefficientSpectrum &operator*=(const CoefficientSpectrum &sp) {
        assert(!sp.HasNaNs());
        for (int i = 0; i < nSpectrumSamples; ++i) c[i] *= sp.c[i];
        return *this;
    }

    CoefficientSpectrum operator*(float a) const {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] *= a;
        assert(!ret.HasNaNs());
        return ret;
    }

    CoefficientSpectrum &operator*=(float a) {
        for (int i = 0; i < nSpectrumSamples; ++i) c[i] *= a;
        assert(!HasNaNs());
        return *this;
    }

    friend inline CoefficientSpectrum operator*(float a,
                                                const CoefficientSpectrum &s) {
        assert(!std::isnan(a) && !s.HasNaNs());
        return s * a;
    }

    CoefficientSpectrum operator/(float a) const {
        assert(a != 0);
        assert(!std::isnan(a));
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] /= a;
        assert(!ret.HasNaNs());
        return ret;
    }

    CoefficientSpectrum &operator/=(float a) {
        assert(a != 0);
        assert(!std::isnan(a));
        for (int i = 0; i < nSpectrumSamples; ++i) c[i] /= a;
        return *this;
    }

    CoefficientSpectrum &operator=(const CoefficientSpectrum &s) {
        assert(!s.HasNaNs());
        for (int i = 0; i < nSpectrumSamples; ++i) c[i] = s.c[i];
        return *this;
    }

    bool operator==(const CoefficientSpectrum &sp) const {
        for (int i = 0; i < nSpectrumSamples; ++i)
            if (c[i] != sp.c[i]) return false;
        return true;
    }

    bool operator!=(const CoefficientSpectrum &sp) const {
        return !(*this == sp);
    }

    bool IsBlack() const {
        for (int i = 0; i < nSpectrumSamples; ++i)
            if (c[i] != 0.) return false;
        return true;
    }

    friend CoefficientSpectrum Sqrt(const CoefficientSpectrum &s) {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] = std::sqrt(s.c[i]);
        assert(!ret.HasNaNs());
        return ret;
    }

    template <int n>
    friend CoefficientSpectrum Pow(const CoefficientSpectrum &s,
                                   float e) {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] = std::pow(s.c[i], e);
        assert(!ret.HasNaNs());
        return ret;
    }
    
    CoefficientSpectrum operator-() const {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] = -c[i];
        return ret;
    }

    friend CoefficientSpectrum Exp(const CoefficientSpectrum &s) {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] = std::exp(s.c[i]);
        assert(!ret.HasNaNs());
        return ret;
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const CoefficientSpectrum &s) {
        return os << s.ToString();
    }

    std::string ToString() const {
        std::ostringstream ss;
        ss << "[ ";

        for (size_t i = 0; i < nSpectrumSamples; ++i) {
            ss << c[i];
            if (i + 1 < nSpectrumSamples) ss << ", ";
        }

        ss << " ]";

        return ss.str();
    }

    CoefficientSpectrum Clamp(float low = 0, float high = Infinity) const {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] = PBRender::Clamp(c[i], low, high);
        assert(!ret.HasNaNs());
        return ret;
    }

    float MaxComponentValue() const {
        float m = c[0];
        for (int i = 1; i < nSpectrumSamples; ++i)
            m = std::max(m, c[i]);
        return m;
    }

    bool HasNaNs() const {
        for (int i = 0; i < nSpectrumSamples; ++i)
            if (std::isnan(c[i])) return true;
        return false;
    }

    float &operator[](int i) {
        assert(i >= 0 && i < nSpectrumSamples);
        return c[i];
    }

    float operator[](int i) const {
        assert(i >= 0 && i < nSpectrumSamples);
        return c[i];
    }

    explicit operator bool() const {
        for (int i = 0; i < nSpectrumSamples; ++i)
            if (c[i] != 0)
                return true;
        return false;
    }

public:
    static const int nSamples = nSpectrumSamples;
    
protected:
    float c[nSpectrumSamples];
};

class RGBSpectrum : public CoefficientSpectrum<3> {
    using CoefficientSpectrum<3>::c;

public:
    // constructors
    RGBSpectrum(float v = 0.f) : CoefficientSpectrum<3>(v) {}
    RGBSpectrum(const CoefficientSpectrum<3> &v) : CoefficientSpectrum<3>(v) {}
    RGBSpectrum(const RGBSpectrum &s, SpectrumType type = SpectrumType::Reflectance) {
        *this =  s;
    }
    RGBSpectrum(const Vector3f &vec) {
        c[0] = vec.x; c[1] = vec.y; c[2] = vec.z;
    }


    static RGBSpectrum FromRGB(const float rgb[3],
                               SpectrumType type = SpectrumType::Reflectance) {
        RGBSpectrum s;
        s.c[0] = rgb[0];
        s.c[1] = rgb[1];
        s.c[2] = rgb[2];
        assert(!s.HasNaNs());
        return s;
    }

    void ToRGB(float *rgb) const {
        rgb[0] = c[0];
        rgb[1] = c[1];
        rgb[2] = c[2];
    }
    
    const RGBSpectrum &ToRGBSpectrum() const { return *this; }
    
    void ToXYZ(float xyz[3]) const { RGBToXYZ(c, xyz); }

    static RGBSpectrum FromXYZ(const float xyz[3],
                                SpectrumType type = SpectrumType::Reflectance) {
        RGBSpectrum r;
        XYZToRGB(xyz, r.c);
        return r;
    }

};

inline RGBSpectrum Lerp(float t, const RGBSpectrum &s1, const RGBSpectrum &s2) {
    return (1 - t) * s1 + t * s2;
}

} // namespace PBRender