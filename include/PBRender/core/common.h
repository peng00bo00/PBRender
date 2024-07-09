#pragma once

#include <assert.h>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <string.h>
#include <vector>

#include <iostream>
#include <sstream>

namespace PBRender
{

// Vector
template <typename T>
struct Vector2;
template <typename T>
struct Vector3;
template <typename T>
struct Point3;
template <typename T>
struct Point2;
template <typename T>
struct Normal3;

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;

using Point2f  = Point2<float>;
using Point2i  = Point2<int>;
using Point3f  = Point3<float>;

// Normal
template <typename T>
struct Normal3;

using Normal3f = Normal3<float>;

// Frame
class Frame;

// Bounds
template <typename T>
class Bounds2;
// template <typename T>
// class Bounds3;

using Bounds2f = Bounds2<float>;
using Bounds2i = Bounds2<int>;

// using Bounds3f = Bounds3<float>;
// using Bounds3i = Bounds3<int>;

// Scene
class Scene;

// Engine
class Engine;
class GeometryViewer;

// Transform
class Transform;

// Color Spectrum
template <int nSpectrumSamples>
class CoefficientSpectrum;

class RGBSpectrum;

using Spectrum = RGBSpectrum;

// Camera
class Camera;
struct CameraSample;
class ProjectiveCamera;
class OrthographicCamera;

// Film
class Film;
class RGBFilm;
class GBuffferFilm;

// Ray
class Ray;

// Sampler
class RNG;
class Sampler;

// TODO: implement other samplers if needed
class IndependentSampler;
class HaltonSampler;
class SobolSampler;

// Integrator
class Integrator;

// BxDF
class BxDF;
struct BSDFSample;
// class BRDF;
// class BTDF;
// class BSDF;

// Containers
template <typename T, typename Alloc>
class Array2D;

// Global Inline Functions
inline uint32_t FloatToBits(float f) {
    uint32_t ui;
    memcpy(&ui, &f, sizeof(float));
    return ui;
}

inline float BitsToFloat(uint32_t ui) {
    float f;
    memcpy(&f, &ui, sizeof(uint32_t));
    return f;
}

inline uint64_t FloatToBits(double f) {
    uint64_t ui;
    memcpy(&ui, &f, sizeof(double));
    return ui;
}

inline double BitsToFloat(uint64_t ui) {
    double f;
    memcpy(&f, &ui, sizeof(uint64_t));
    return f;
}

inline float NextFloatUp(float v) {
    // Handle infinity and negative zero for _NextFloatUp()_
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;

    // Advance _v_ to next higher float
    uint32_t ui = FloatToBits(v);
    if (v >= 0)
        ++ui;
    else
        --ui;
    return BitsToFloat(ui);
}

inline float NextFloatDown(float v) {
    // Handle infinity and positive zero for _NextFloatDown()_
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint32_t ui = FloatToBits(v);
    if (v > 0)
        --ui;
    else
        ++ui;
    return BitsToFloat(ui);
}

inline double NextFloatUp(double v, int delta = 1) {
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;
    uint64_t ui = FloatToBits(v);
    if (v >= 0.)
        ui += delta;
    else
        ui -= delta;
    return BitsToFloat(ui);
}

inline double NextFloatDown(double v, int delta = 1) {
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint64_t ui = FloatToBits(v);
    if (v > 0.)
        ui -= delta;
    else
        ui += delta;
    return BitsToFloat(ui);
}

// Optional
struct BadOptionalAccess : std::exception {
    BadOptionalAccess() = default;
    virtual ~BadOptionalAccess() = default;

    const char *what() const noexcept override {
        return "BadOptionalAccess";
    }
};

struct Nullopt {
    explicit Nullopt() = default;
};

inline constexpr Nullopt nullopt;

template <typename T>
class Optional {
public:
    using value_type = T;

    // constructors
    Optional() noexcept : m_has_value(false) {}
    Optional(Nullopt) noexcept : m_has_value(false) {}

    Optional(T &&value) noexcept : m_has_value(true), m_value(std::move(value)) {}
    Optional(T const &value) noexcept : m_has_value(true), m_value(std::move(value)) {}

    Optional(Optional const &that) : m_has_value(that.has_value()) {
        if (m_has_value) {
            new (&m_value) T(that.m_value); // placement-new
        }
    }
    Optional(Optional &&that) noexcept : m_has_value(that.has_value()) {
        if (m_has_value) {
            new (&m_value) T(std::move(that.m_value));
            that.reset();
        }
    }

    Optional &operator=(Nullopt) noexcept {
        reset();
        return *this;
    }

    Optional &operator=(T &&value) noexcept {
        reset();
        new (&m_value) T(std::move(value));
        m_has_value = true;

        return *this;
    }

    Optional &operator=(T const &value) noexcept {
        reset();
        new (&m_value) T(value);
        m_has_value = true;

        return *this;
    }

    Optional &operator=(Optional const &that) {
        if (this == &that) return *this;

        reset();
        if (that.has_value()) {
            new (&m_value) T(that.m_value);
            m_has_value = true;
        }

        return *this;
    }

    Optional &operator=(Optional &&that) noexcept {
        if (this == &that) return *this;

        reset();
        if (that.has_value()) {
            new (&m_value) T(std::move(that.m_value));
            m_has_value = true;
            that.reset();
        }
        
        return *this;
    }

    // destructors
    void reset() noexcept {
        if (m_has_value) {
            m_value.~T();       // placement-delete
            m_has_value = false;
        }
    }

    ~Optional() noexcept { reset(); }

    bool has_value() const noexcept {
        return m_has_value;
    }

    explicit operator bool() const noexcept {
        return m_has_value;
    }

    bool operator==(Nullopt) const noexcept {
        return !m_has_value;
    }

    friend bool operator==(Nullopt, Optional const &self) noexcept {
        return !self.m_has_value;
    }

    bool operator!=(Nullopt) const noexcept {
        return m_has_value;
    }

    friend bool operator!=(Nullopt, Optional const &self) noexcept {
        return self.m_has_value;
    }

    bool operator==(Optional<T> const &that) const noexcept {
        if (m_has_value != that.has_value())
            return false;
        if (m_has_value) {
            return m_value == that.m_value;
        }
        return true;
    }

    bool operator!=(Optional const &that) const noexcept {
        if (m_has_value != that.has_value())
            return true;
        if (m_has_value) {
            return m_value != that.m_value;
        }
        return false;
    }

    T const &value() const & {
        if (!m_has_value)
            throw BadOptionalAccess();
        return m_value;
    }

    T &value() & {
        if (!m_has_value)
            throw BadOptionalAccess();
        return m_value;
    }

    T const &&value() const && {
        if (!m_has_value)
            throw BadOptionalAccess();
        return std::move(m_value);
    }

    T &&value() && {
        if (!m_has_value)
            throw BadOptionalAccess();
        return std::move(m_value);
    }

        T const &operator*() const & noexcept {
        return m_value;
    }

    T &operator*() & noexcept {
        return m_value;
    }

    T const &&operator*() const && noexcept {
        return std::move(m_value);
    }

    T &&operator*() && noexcept {
        return std::move(m_value);
    }

    T const *operator->() const noexcept {
        return &m_value;
    }

    T *operator->() noexcept {
        return &m_value;
    }

private:
    bool m_has_value;
    union {
        T m_value;
    };
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Optional<T> &opt) {
    if (opt.has_value())
        return os << "[ Optional<" << typeid(T).name() << "> set: true "
                  << "value: " << opt.value() << " ]";
    else
        return os << "[ Optional<" << typeid(T).name()
                  << "> set: false value: n/a ]";
}

} // namespace PBRender
