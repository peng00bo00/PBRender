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

#ifndef NDEBUG
    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the Assert checks.
    Vector2(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
    }

    Vector2<T> &operator=(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
        return *this;
    }
#endif  // !NDEBUG

        Vector2<T> operator+(const Vector2<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Vector2(x + v.x, y + v.y);
        }

        Vector2<T> &operator+=(const Vector2<T> &v) {
            DCHECK(!v.HasNaNs());
            x += v.x;
            y += v.y;

            return *this;
        }

        Vector2<T> operator-(const Vector2<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Vector2(x - v.x, y - v.y);
        }

        Vector2<T> &operator-=(const Vector2<T> &v) {
            DCHECK(!v.HasNaNs());
            x -= v.x;
            y -= v.y;

            return *this;
        }

        bool operator==(const Vector2<T> &v) const { return x == v.x && y == v.y; }
        bool operator!=(const Vector2<T> &v) const { return x != v.x || y != v.y; }

        template <typename U>
        Vector2<T> operator*(U f) const {
            return Vector2<T>(f * x, f * y);
        }

        template <typename U>
        Vector2<T> &operator*=(U f) {
            DCHECK(!isNaN(f));
            x *= f;
            y *= f;
            return *this;
        }

        template <typename U>
        Vector2<T> operator/(U f) const {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            return Vector2<T>(x * inv, y * inv);
        }

        template <typename U>
        Vector2<T> &operator/=(U f) {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            x *= inv;
            y *= inv;
            return *this;
        }

        Vector2<T> operator-() const {
            return Vector2<T>(-x, -y); 
        }

        T operator[](int i) const {
            DCHECK(i >= 0 && i <= 1);
            if (i == 0) return x;
            return y;
        }

        T &operator[](int i) {
            DCHECK(i >= 0 && i <= 1);
            if (i == 0) return x;
            return y;
        }

        Float LengthSquared() const { return x * x + y * y; }
        Float Length() const { return std::sqrt(LengthSquared()); }

        // Vector2 data
        T x, y;
};

template <typename T>
class Vector3 {
    public:
        Vector3() { x = y = z = 0; }
        Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { DCHECK(!HasNaNs()); }
        bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
        explicit Vector3(const Point3<T> &p);

#ifndef NDEBUG
    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the Assert checks.
    Vector3(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector3<T> &operator=(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
#endif  // !NDEBUG

        Vector3<T> operator+(const Vector3<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        Vector3<T> &operator+=(const Vector3<T> &v) {
            DCHECK(!v.HasNaNs());
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3<T> operator-(const Vector3<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        Vector3<T> &operator-=(const Vector3<T> &v) {
            DCHECK(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        bool operator==(const Vector3<T> &v) const {
            return x == v.x && y == v.y && z == v.z;
        }

        bool operator!=(const Vector3<T> &v) const {
            return x != v.x || y != v.y || z != v.z;
        }

        template <typename U>
        Vector3<T> operator*(U s) const {
            return Vector3<T>(s * x, s * y, s * z);
        }

        template <typename U>
        Vector3<T> &operator*=(U s) {
            DCHECK(!isNaN(s));
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        template <typename U>
        Vector3<T> operator/(U f) const {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            return Vector3<T>(x * inv, y * inv, z * inv);
        }

        template <typename U>
        Vector3<T> &operator/=(U f) {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        Vector3<T> operator-() const { 
            return Vector3<T>(-x, -y, -z); 
        }

        Float LengthSquared() const { 
            return x * x + y * y + z * z; 
        }

        Float Length() const { return std::sqrt(LengthSquared()); }
        explicit Vector3(const Normal3<T> &n);

        // Vector3 data
        T x, y, z;
};

typedef Vector2<Float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<Float> Vector3f;
typedef Vector3<int> Vector3i;

// Point Declarations
template <typename T>
class Point2 {

};

class Point3 {

};

}