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
        Vector2(T xx, T yy) : x(xx), y(yy) { DCHECK(!HasNaNs()); }

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
            if (i == 0) return x;
            return y;
        }

        T &operator[](int i) {
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

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

}