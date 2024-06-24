#pragma once

#include <Eigen/Eigen>
#include <Eigen/Geometry>

#include <PBRender/core/common.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/ray.h>

namespace PBRender
{

class Transform {
public:
    Transform() 
            : m(Eigen::Matrix4f::Identity()),
              mInv(Eigen::Matrix4f::Identity()) {}
    
    Transform(const Eigen::Matrix4f &matrix) 
            : m(matrix), mInv(matrix.inverse()) {}
    Transform(const Eigen::Matrix4f &matrix, const Eigen::Matrix4f &matrixInv)
            : m(matrix), mInv(matrixInv) {}

    Transform Inverse() const {
        return Transform{mInv, m};
    }

    bool operator==(const Transform &t) const {
        return t.m == m && t.mInv == mInv;
    }

    bool operator!=(const Transform &t) const {
        return t.m != m || t.mInv != mInv;
    }

    friend std::ostream &operator<<(std::ostream &os, const Transform &Trans) {
        // clang-format off
        os << "Transform\n"
           << "[ [ " << Trans.m(0, 0) << ", " << Trans.m(0, 1) << ", " << Trans.m(0, 2) << ", " << Trans.m(0, 3) << " ]\n"
           << "  [ " << Trans.m(1, 0) << ", " << Trans.m(1, 1) << ", " << Trans.m(1, 2) << ", " << Trans.m(1, 3) << " ]\n"
           << "  [ " << Trans.m(2, 0) << ", " << Trans.m(2, 1) << ", " << Trans.m(2, 2) << ", " << Trans.m(2, 3) << " ]\n"
           << "  [ " << Trans.m(3, 0) << ", " << Trans.m(3, 1) << ", " << Trans.m(3, 2) << ", " << Trans.m(3, 3) << " ] ]\n"
           << "InvTransform\n"
           << "[ [ " << Trans.mInv(0, 0) << ", " << Trans.mInv(0, 1) << ", " << Trans.mInv(0, 2) << ", " << Trans.mInv(0, 3) << " ]\n"
           << "  [ " << Trans.mInv(1, 0) << ", " << Trans.mInv(1, 1) << ", " << Trans.mInv(1, 2) << ", " << Trans.mInv(1, 3) << " ]\n"
           << "  [ " << Trans.mInv(2, 0) << ", " << Trans.mInv(2, 1) << ", " << Trans.mInv(2, 2) << ", " << Trans.mInv(2, 3) << " ]\n"
           << "  [ " << Trans.mInv(3, 0) << ", " << Trans.mInv(3, 1) << ", " << Trans.mInv(3, 2) << ", " << Trans.mInv(3, 3) << " ] ]";

        return os;
    }
    
    const Eigen::Matrix4f &GetMatrix() const { return m; }
    const Eigen::Matrix4f &GetInverseMatrix() const { return mInv; }
    
    // operators
    template <typename T>
    inline Point3<T> operator()(const Point3<T> &p) const;

    template <typename T>
    inline Vector3<T> operator()(const Vector3<T> &v) const;

    template <typename T>
    inline Normal3<T> operator()(const Normal3<T> &) const;

    inline Ray operator()(const Ray &r) const;

    inline Transform operator*(const Transform &t2) const {
        return {m * t2.m, t2.mInv * mInv};
    }

private:
    Eigen::Matrix4f m, mInv;
};



// Transform Inline Functions
template <typename T>
inline Point3<T> Transform::operator()(const Point3<T> &p) const {
    Eigen::Matrix<T, 4, 1> pp(p.x, p.y, p.z, T(1.0f));
    pp = m * pp;
    assert((float)pp(3) != 0.0f);

    if (pp(0, 3) == 1.0f)
        return Point3<T>(pp(0), pp(1), pp(2));
    else
        return Point3<T>(pp(0), pp(1), pp(2)) / pp(3);
}

template <typename T>
inline Vector3<T> Transform::operator()(const Vector3<T> &v) const {
    Eigen::Matrix<T, 3, 1> vv = v.ToEigen();
    vv = m.topLeftCorner<3, 3>() * vv;

    return Vector3<T>(vv);
}

template <typename T>
inline Normal3<T> Transform::operator()(const Normal3<T> &n) const {
    Eigen::Matrix<T, 3, 1> nn = n.ToEigen();
    // note to use the mInv.T in the transformation
    nn = mInv.topLeftCorner<3, 3>().transpose() * nn;

    return Normal3<T>(nn);
}

inline Ray Transform::operator()(const Ray &r) const {
    Point3f org  = (*this)(r.org);
    Vector3f dir = (*this)(r.dir);

    return Ray(org, dir, r.tnear, r.tfar);
}

// useful transform constructors
Transform Translate(const Vector3f &delta);
Transform Scale(float x, float y, float z);
Transform RotateX(float theta);
Transform RotateY(float theta);
Transform RotateZ(float theta);
Transform AxisAngle(float theta, const Vector3f &axis);
Transform LookAt(const Point3f &pos, const Point3f &look, const Vector3f &up);
Transform Orthographic(float zNear, float zFar);
Transform Perspective(float fov, float zNear, float zFar);

} // namespace PBRender