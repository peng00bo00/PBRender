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
inline std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Vector2<Float> &v) {
    os << StringPrintf("[ %f, %f ]", v.x, v.y);
    return os;
}

template <typename T>
class Vector3 {
    public:
        Vector3() { x = y = z = 0; }
        Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { DCHECK(!HasNaNs()); }
        bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
        explicit Vector3(const Point3<T> &p);

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

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Vector3<Float> &v) {
    os << StringPrintf("[ %f, %f, %f ]", v.x, v.y, v.z);
    return os;
}

typedef Vector2<Float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<Float> Vector3f;
typedef Vector3<int> Vector3i;

// Point Declarations
template <typename T>
class Point2 {
    public:
        explicit Point2(const Point3<T> &p) : x(p.x), y(p.y) { DCHECK(!HasNaNs()); }
        Point2() { x = y = 0; }
        Point2(T xx, T yy) : x(xx), y(yy) { DCHECK(!HasNaNs()); }

        template <typename U>
        explicit Point2(const Point2<U> &p) {
            x = (T)p.x;
            y = (T)p.y;
            DCHECK(!HasNaNs());
        }

        template <typename U>
        explicit Point2(const Vector2<U> &p) {
            x = (T)p.x;
            y = (T)p.y;
            DCHECK(!HasNaNs());
        }

        template <typename U>
        explicit operator Vector2<U>() const {
            return Vector2<U>(x, y);
        }

        Point2(const Point2<T> &p) {
            DCHECK(!p.HasNaNs());
            x = p.x;
            y = p.y;
        }

        Point2<T> &operator=(const Point2<T> &p) {
            DCHECK(!p.HasNaNs());
            x = p.x;
            y = p.y;
            return *this;
        }

        Point2<T> operator+(const Vector2<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Point2<T>(x + v.x, y + v.y);
        }

        Point2<T> &operator+=(const Vector2<T> &v) {
            DCHECK(!v.HasNaNs());
            x += v.x;
            y += v.y;
            return *this;
        }

        Vector2<T> operator-(const Point2<T> &p) const {
            DCHECK(!p.HasNaNs());
            return Vector2<T>(x - p.x, y - p.y);
        }

        Point2<T> operator-(const Vector2<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Point2<T>(x - v.x, y - v.y);
        }

        Point2<T> operator-() const { return Point2<T>(-x, -y); }

        Point2<T> &operator-=(const Vector2<T> &v) {
            DCHECK(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            return *this;
        }

        Point2<T> &operator+=(const Point2<T> &p) {
            DCHECK(!p.HasNaNs());
            x += p.x;
            y += p.y;
            return *this;
        }

        Point2<T> operator+(const Point2<T> &p) const {
            DCHECK(!p.HasNaNs());
            return Point2<T>(x + p.x, y + p.y);
        }

        template <typename U>
        Point2<T> operator*(U f) const {
            return Point2<T>(f * x, f * y);
        }

        template <typename U>
        Point2<T> &operator*=(U f) {
            x *= f;
            y *= f;
            return *this;
        }

        template <typename U>
        Point2<T> operator/(U f) const {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            return Point2<T>(inv * x, inv * y);
        }

        template <typename U>
        Point2<T> &operator/=(U f) {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            x *= inv;
            y *= inv;
            return *this;
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

        bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
        bool operator!=(const Point2<T> &p) const { return x != p.x || y != p.y; }
        bool HasNaNs() const { return isNaN(x) || isNaN(y); }

        // Point2 data
        T x, y;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point2<T> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Point2<Float> &v) {
    os << StringPrintf("[ %f, %f ]", v.x, v.y);
    return os;
}

template <typename T>
class Point3 {
    public:
        Point3() { x = y = z = 0; }
        Point3(T x, T y, T z) : x(x), y(y), z(z) { DCHECK(!HasNaNs()); }

        template <typename U>
        explicit Point3(const Point3<U> &p) : x((T)p.x), y((T)p.y), z((T)p.z) {
            DCHECK(!HasNaNs());
        }

        template <typename U>
        explicit operator Vector3<U>() const {
            return Vector3<U>(x, y, z);
        }

        Point3(const Point3<T> &p) {
            DCHECK(!p.HasNaNs());
            x = p.x;
            y = p.y;
            z = p.z;
        }

        Point3<T> &operator=(const Point3<T> &p) {
            DCHECK(!p.HasNaNs());
            x = p.x;
            y = p.y;
            z = p.z;
            return *this;
        }

        Point3<T> operator+(const Vector3<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Point3<T>(x + v.x, y + v.y, z + v.z);
        }

        Point3<T> &operator+=(const Vector3<T> &v) {
            DCHECK(!v.HasNaNs());
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3<T> operator-(const Point3<T> &p) const {
            DCHECK(!p.HasNaNs());
            return Vector3<T>(x - p.x, y - p.y, z - p.z);
        }

        Point3<T> operator-(const Vector3<T> &v) const {
            DCHECK(!v.HasNaNs());
            return Point3<T>(x - v.x, y - v.y, z - v.z);
        }

        Point3<T> &operator-=(const Vector3<T> &v) {
            DCHECK(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        Point3<T> &operator+=(const Point3<T> &p) {
            DCHECK(!p.HasNaNs());
            x += p.x;
            y += p.y;
            z += p.z;
            return *this;
        }

        Point3<T> operator+(const Point3<T> &p) const {
            DCHECK(!p.HasNaNs());
            return Point3<T>(x + p.x, y + p.y, z + p.z);
        }

        template <typename U>
        Point3<T> operator*(U f) const {
            return Point3<T>(f * x, f * y, f * z);
        }

        template <typename U>
        Point3<T> &operator*=(U f) {
            x *= f;
            y *= f;
            z *= f;
            return *this;
        }

        template <typename U>
        Point3<T> operator/(U f) const {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            return Point3<T>(inv * x, inv * y, inv * z);
        }

        template <typename U>
        Point3<T> &operator/=(U f) {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        T operator[](int i) const {
            DCHECK(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T &operator[](int i) {
            DCHECK(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        bool operator==(const Point3<T> &p) const {
            return x == p.x && y == p.y && z == p.z;
        }

        bool operator!=(const Point3<T> &p) const {
            return x != p.x || y != p.y || z != p.z;
        }

        bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
        Point3<T> operator-() const { return Point3<T>(-x, -y, -z); }

        // Point3 data
        T x, y, z;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Point3<Float> &v) {
    os << StringPrintf("[ %f, %f, %f ]", v.x, v.y, v.z);
    return os;
}

typedef Point2<Float> Point2f;
typedef Point2<int> Point2i;
typedef Point3<Float> Point3f;
typedef Point3<int> Point3i;

// Normal Declarations
template <typename T>
class Normal3 {
    public:
        Normal3() { x = y = z = 0; }
        Normal3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { DCHECK(!HasNaNs()); }

        Normal3<T> operator-() const { 
            return Normal3(-x, -y, -z); 
        }

        Normal3<T> operator+(const Normal3<T> &n) const {
            DCHECK(!n.HasNaNs());
            return Normal3<T>(x + n.x, y + n.y, z + n.z);
        }

        Normal3<T> &operator+=(const Normal3<T> &n) {
            DCHECK(!n.HasNaNs());
            x += n.x;
            y += n.y;
            z += n.z;
            return *this;
        }

        Normal3<T> operator-(const Normal3<T> &n) const {
            DCHECK(!n.HasNaNs());
            return Normal3<T>(x - n.x, y - n.y, z - n.z);
        }

        Normal3<T> &operator-=(const Normal3<T> &n) {
            DCHECK(!n.HasNaNs());
            x -= n.x;
            y -= n.y;
            z -= n.z;
            return *this;
        }

        bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }

        template <typename U>
        Normal3<T> operator*(U f) const {
            return Normal3<T>(f * x, f * y, f * z);
        }

        template <typename U>
        Normal3<T> &operator*=(U f) {
            x *= f;
            y *= f;
            z *= f;
            return *this;
        }

        template <typename U>
        Normal3<T> operator/(U f) const {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            return Normal3<T>(x * inv, y * inv, z * inv);
        }

        template <typename U>
        Normal3<T> &operator/=(U f) {
            CHECK_NE(f, 0);
            Float inv = (Float)1 / f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        Float LengthSquared() const { return x * x + y * y + z * z; }
        Float Length() const { return std::sqrt(LengthSquared()); }

        Normal3<T>(const Normal3<T> &n) {
            DCHECK(!n.HasNaNs());
            x = n.x;
            y = n.y;
            z = n.z;
        }

        Normal3<T> &operator=(const Normal3<T> &n) {
            DCHECK(!n.HasNaNs());
            x = n.x;
            y = n.y;
            z = n.z;
            return *this;
        }

        explicit Normal3<T>(const Vector3<T> &v) : x(v.x), y(v.y), z(v.z) {
            DCHECK(!v.HasNaNs());
        }

        bool operator==(const Normal3<T> &n) const {
            return x == n.x && y == n.y && z == n.z;
        }

        bool operator!=(const Normal3<T> &n) const {
            return x != n.x || y != n.y || z != n.z;
        }

        T operator[](int i) const {
            DCHECK(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T &operator[](int i) {
            DCHECK(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T x, y, z;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Normal3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Normal3<Float> &v) {
    os << StringPrintf("[ %f, %f, %f ]", v.x, v.y, v.z);
    return os;
}

typedef Normal3<Float> Normal3f;

// Bounds Declarations
template <typename T>
class Bounds2 {
    public:
        Bounds2() {
            T minNum = std::numeric_limits<T>::lowest();
            T maxNum = std::numeric_limits<T>::max();
            pMin = Point2<T>(maxNum, maxNum);
            pMax = Point2<T>(minNum, minNum);
        }

        explicit Bounds2(const Point2<T> &p) : pMin(p), pMax(p) {}

        Bounds2(const Point2<T> &p1, const Point2<T> &p2) {
            pMin = Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
            pMax = Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
        }

        template <typename U>
        explicit operator Bounds2<U>() const {
            return Bounds2<U>((Point2<U>)pMin, (Point2<U>)pMax);
        }

        Vector2<T> Diagonal() const {
            return pMax - pMin;
        }

        T Area() const {
            Vector2<T> d = pMax - pMin;
            return (d.x * d.y);
        }

        int MaximumExtent() const {
            Vector2<T> diag = Diagonal();
            if (diag.x > diag.y)
                return 0;
            else
                return 1;
        }

        inline const Point2<T> &operator[](int i) const {
            DCHECK(i == 0 || i == 1);
            return (i == 0) ? pMin : pMax;
        }

        inline Point2<T> &operator[](int i) {
            DCHECK(i == 0 || i == 1);
            return (i == 0) ? pMin : pMax;
        }

        bool operator==(const Bounds2<T> &b) const {
            return b.pMin == pMin && b.pMax == pMax;
        }

        bool operator!=(const Bounds2<T> &b) const {
            return b.pMin != pMin || b.pMax != pMax;
        }

        Point2<T> Lerp(const Point2f &t) const {
            return Point2<T>(pbrt::Lerp(t.x, pMin.x, pMax.x),
                             pbrt::Lerp(t.y, pMin.y, pMax.y));
        }

        Vector2<T> Offset(const Point2<T> &p) const {
            Vector2<T> o = p - pMin;
            if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
            if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
            return o;
        }

        void BoundingSphere(Point2<T> *c, Float *rad) const {
            *c = (pMin + pMax) / 2;
            *rad = Inside(*c, *this) ? Distance(*c, pMax) : 0;
        }

        friend std::ostream &operator<<(std::ostream &os, const Bounds2<T> &b) {
            os << "[ " << b.pMin << " - " << b.pMax << " ]";
            return os;
        }

        // Bounds2 data
        Point2<T> pMin, pMax;
};

template <typename T>
class Bounds3 {
    public:
        Bounds3() {
            T minNum = std::numeric_limits<T>::lowest();
            T maxNum = std::numeric_limits<T>::max();
            pMin = Point3<T>(maxNum, maxNum, maxNum);
            pMax = Point3<T>(minNum, minNum, minNum);
        }

        explicit Bounds3(const Point3<T> &p) : pMin(p), pMax(p) {}

        Bounds3(const Point3<T> &p1, const Point3<T> &p2)
            : pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)),
              pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)) {}

        const Point3<T> &operator[](int i) const;

        Point3<T> &operator[](int i);

        bool operator==(const Bounds3<T> &b) const {
            return b.pMin == pMin && b.pMax == pMax;
        }

        bool operator!=(const Bounds3<T> &b) const {
            return b.pMin != pMin || b.pMax != pMax;
        }

        Point3<T> Corner(int corner) const {
            DCHECK(corner >= 0 && corner < 8);
            return Point3<T>((*this)[(corner & 1)].x,
                            (*this)[(corner & 2) ? 1 : 0].y,
                            (*this)[(corner & 4) ? 1 : 0].z);
        }

        Vector3<T> Diagonal() const {
            return pMax - pMin;
        }

        T SurfaceArea() const {
            Vector3<T> d = Diagonal();
            return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
        }

        T Volume() const {
            Vector3<T> d = Diagonal();
            return d.x * d.y * d.z;
        }

        int MaximumExtent() const {
            Vector3<T> d = Diagonal();
            if (d.x > d.y && d.x > d.z)
                return 0;
            else if (d.y > d.z)
                return 1;
            else
                return 2;
        }

        Point3<T> Lerp(const Point3f &t) const {
            return Point3<T>(pbrt::Lerp(t.x, pMin.x, pMax.x),
                             pbrt::Lerp(t.y, pMin.y, pMax.y),
                             pbrt::Lerp(t.z, pMin.z, pMax.z));
        }

        Vector3<T> Offset(const Point3<T> &p) const {
            Vector3<T> o = p - pMin;
            if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
            if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
            if (pMax.z > pMin.z) o.z /= pMax.z - pMin.z;
            return o;
        }

        void BoundingSphere(Point3<T> *center, Float *radius) const {
            *center = (pMin + pMax) / 2;
            *radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
        }

        template <typename U>
        explicit operator Bounds3<U>() const {
            return Bounds3<U>((Point3<U>)pMin, (Point3<U>)pMax);
        }

        bool IntersectP(const Ray &ray, Float *hitt0 = nullptr, Float *hitt1 = nullptr) const;

        inline bool IntersectP(const Ray &ray, const Vector3f &invDir, const int dirIsNeg[3]) const;

        friend std::ostream &operator<<(std::ostream &os, const Bounds3<T> &b) {
            os << "[ " << b.pMin << " - " << b.pMax << " ]";
            return os;
        }

        // Bounds3 data
        Point3<T> pMin, pMax;
};

typedef Bounds2<Float> Bounds2f;
typedef Bounds2<int> Bounds2i;
typedef Bounds3<Float> Bounds3f;
typedef Bounds3<int> Bounds3i;


}