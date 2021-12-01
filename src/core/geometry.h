#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CORE_GEOMETRY_H
#define PBRT_CORE_GEOMETRY_H

// core/geometry.h*
#include "pbrt.h"
#include "stringprint.h"
#include <iterator>


namespace pbrt {

template <typename T>
inline bool isNaN(const T x) {
    return std::isnan(x);
}

template <>
inline bool isNaN(const int x) {
    return false;
}

// Vector Declarations
template <typename T>
class Vector2 {
    public:
        Vector2() { x = y = 0; }
        Vector2(T xx, T yy) : x(xx), y(yy) { DCHECK(!HasNaNs()); }
        bool HasNaNs() const { return isNaN(x) || isNaN(y); }
        explicit Vector2(const Point2<T> &p);
        explicit Vector2(const Point3<T> &p);
}

}