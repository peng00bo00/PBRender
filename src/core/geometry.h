#pragma once

#include "PBRender.h"

namespace PBRender {

template <typename T>
inline bool isNaN(const T x) {
    return std::isnan(x);
}
template <>
inline bool isNaN(const int x) {
    return false;
}

template <typename T>
class Vector2 {
    public:
        Vector2() { x = y = 0; }
        Vector2(T xx, T yy) : x(xx), y(yy) { assert(!HasNaNs()); }

        explicit Vector2(const Point2<T> &p);
        explicit Vector2(const Point3<T> &p);

        bool HasNaNs() const { return isNaN(x) || isNaN(y); }

        Vector2(const Vector2<T> &v) {
            x = v.x;
            y = v.y;
        }

        Vector2<T> &operator=(const Vector2<T> &v) {
            x = v.x;
            y = v.y;
            return *this;
        }

        Vector2<T> operator+(const Vector2<T> &v) const {
            return Vector2(x + v.x, y + v.y);
        }

        Vector2<T> &operator+=(const Vector2<T> &v) {
            x += v.x;
            y += v.y;
            return *this;
        }
        Vector2<T> operator-(const Vector2<T> &v) const {
            return Vector2(x - v.x, y - v.y);
        }

        Vector2<T> &operator-=(const Vector2<T> &v) {
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
            x *= f;
            y *= f;
            return *this;
        }

        template <typename U>
        Vector2<T> operator/(U f) const {
            float inv = (float)1 / f;
            return Vector2<T>(x * inv, y * inv);
        }

        template <typename U>
        Vector2<T> &operator/=(U f) {
            float inv = (float)1 / f;
            x *= inv;
            y *= inv;
            return *this;
        }

        Vector2<T> operator-() const { return Vector2<T>(-x, -y); }

        T operator[](int i) const {
            assert(i == 0 || i == 1);
            if (i == 0) return x;
            return y;
        }

        T &operator[](int i) {
            assert(i == 0 || i == 1);
            if (i == 0) return x;
            return y;
        }

        float LengthSquared() const { return x * x + y * y; }
        float Length() const { return std::sqrt(LengthSquared()); }

    public:
        T x, y;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <typename T>
class Vector3 {
    public:
        Vector3() { x = y = z = 0; }
        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

        bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }

        Vector3(const Vector3<T> &v) {
            assert(!v.HasNaNs());
            x = v.x;
            y = v.y;
            z = v.z;
        }

        Vector3<T> &operator=(const Vector3<T> &v) {
            assert(!v.HasNaNs());
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        Vector3<T> operator+(const Vector3<T> &v) const {
            assert(!v.HasNaNs());
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        Vector3<T> &operator+=(const Vector3<T> &v) {
            assert(!v.HasNaNs());
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3<T> operator-(const Vector3<T> &v) const {
            assert(!v.HasNaNs());
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        Vector3<T> &operator-=(const Vector3<T> &v) {
            assert(!v.HasNaNs());
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
            assert(!isNaN(s));
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        template <typename U>
        Vector3<T> operator/(U f) const {
            assert( (float)f != 0.0f );
            float inv = (float)1 / f;
            return Vector3<T>(x * inv, y * inv, z * inv);
        }

        template <typename U>
        Vector3<T> &operator/=(U f) {
            assert( (float)f != 0.0f );
            float inv = (float)1 / f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
        
        T operator[](int i) const {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T &operator[](int i) {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }
        
        float LengthSquared() const { return x * x + y * y + z * z; }
        float Length() const { return std::sqrt(LengthSquared()); }
    
    public:
        T x, y, z;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

template <typename T>
class Point2 {
    public:
        Point2() { x = y = 0; }
        Point2(T xx, T yy) : x(xx), y(yy) { assert(!HasNaNs()); }
        explicit Point2(const Point3<T> &p) : x(p.x), y(p.y) { assert(!HasNaNs()); }

        template <typename U>
        explicit Point2(const Point2<U> &p) {
            x = (T)p.x;
            y = (T)p.y;
            assert(!HasNaNs());
        }

        template <typename U>
        explicit Point2(const Vector2<U> &p) {
            x = (T)p.x;
            y = (T)p.y;
            assert(!HasNaNs());
        }

        template <typename U>
        explicit operator Vector2<U>() const {
            return Vector2<U>(x, y);
        }

        Point2(const Point2<T> &p) {
            assert(!p.HasNaNs());
            x = p.x;
            y = p.y;
        }

        Point2<T> &operator=(const Point2<T> &p) {
            assert(!p.HasNaNs());
            x = p.x;
            y = p.y;
            return *this;
        }

        Point2<T> operator+(const Vector2<T> &v) const {
            assert(!v.HasNaNs());
            return Point2<T>(x + v.x, y + v.y);
        }

        Point2<T> &operator+=(const Vector2<T> &v) {
            assert(!v.HasNaNs());
            x += v.x;
            y += v.y;
            return *this;
        }
        Vector2<T> operator-(const Point2<T> &p) const {
            assert(!p.HasNaNs());
            return Vector2<T>(x - p.x, y - p.y);
        }

        Point2<T> operator-(const Vector2<T> &v) const {
            assert(!v.HasNaNs());
            return Point2<T>(x - v.x, y - v.y);
        }

        Point2<T> operator-() const { return Point2<T>(-x, -y); }

        Point2<T> &operator-=(const Vector2<T> &v) {
            assert(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            return *this;
        }
        Point2<T> &operator+=(const Point2<T> &p) {
            assert(!p.HasNaNs());
            x += p.x;
            y += p.y;
            return *this;
        }

        Point2<T> operator+(const Point2<T> &p) const {
            assert(!p.HasNaNs());
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
            assert((float)f != 0.0f );
            float inv = (float)1 / f;
            return Point2<T>(inv * x, inv * y);
        }

        template <typename U>
        Point2<T> &operator/=(U f) {
            assert((float)f != 0.0f );
            float inv = (float)1 / f;
            x *= inv;
            y *= inv;
            return *this;
        }

        T operator[](int i) const {
            assert(i >= 0 && i <= 1);
            if (i == 0) return x;
            return y;
        }

        T &operator[](int i) {
            assert(i >= 0 && i <= 1);
            if (i == 0) return x;
            return y;
        }

        bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
        bool operator!=(const Point2<T> &p) const { return x != p.x || y != p.y; }
        bool HasNaNs() const { return isNaN(x) || isNaN(y); }
    
    public:
        T x, y;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point2<T> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <typename T>
class Point3 {
    public:
        Point3() { x = y = z = 0; }
        Point3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { assert(!HasNaNs()); }

        template <typename U>
        explicit Point3(const Point3<U> &p)
            : x((T)p.x), y((T)p.y), z((T)p.z) {
            assert(!HasNaNs());
        }

        template <typename U>
        explicit operator Vector3<U>() const {
            return Vector3<U>(x, y, z);
        }

        Point3(const Point3<T> &p) {
            assert(!p.HasNaNs());
            x = p.x;
            y = p.y;
            z = p.z;
        }

        Point3<T> &operator=(const Point3<T> &p) {
            assert(!p.HasNaNs());
            x = p.x;
            y = p.y;
            z = p.z;
            return *this;
        }

        Point3<T> operator+(const Vector3<T> &v) const {
            assert(!v.HasNaNs());
            return Point3<T>(x + v.x, y + v.y, z + v.z);
        }
        Point3<T> &operator+=(const Vector3<T> &v) {
            assert(!v.HasNaNs());
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3<T> operator-(const Point3<T> &p) const {
            assert(!p.HasNaNs());
            return Vector3<T>(x - p.x, y - p.y, z - p.z);
        }

        Point3<T> operator-(const Vector3<T> &v) const {
            assert(!v.HasNaNs());
            return Point3<T>(x - v.x, y - v.y, z - v.z);
        }

        Point3<T> &operator-=(const Vector3<T> &v) {
            assert(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        Point3<T> &operator+=(const Point3<T> &p) {
            assert(!p.HasNaNs());
            x += p.x;
            y += p.y;
            z += p.z;
            return *this;
        }
        Point3<T> operator+(const Point3<T> &p) const {
            assert(!p.HasNaNs());
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
            assert((float)f != 0.0f);
            float inv = (float)1 / f;
            return Point3<T>(inv * x, inv * y, inv * z);
        }

        template <typename U>
        Point3<T> &operator/=(U f) {
            assert((float)f != 0.0f);
            float inv = (float)1 / f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        T operator[](int i) const {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T &operator[](int i) {
            assert(i >= 0 && i <= 2);
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
    
    public:
        T x, y, z;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

typedef Point2<float> Point2f;
typedef Point2<int> Point2i;
typedef Point3<float> Point3f;
typedef Point3<int> Point3i;

class Ray {
    public:
        Ray() {}
    
    public:
        Point3f o;
        Vector3f d;
};

}