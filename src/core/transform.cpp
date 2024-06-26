#include <PBRender/core/transform.h>
#include <PBRender/core/math.h>

namespace PBRender
{
Transform Translate(const Vector3f &delta) {
    Eigen::Matrix4f m    = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f mInv = Eigen::Matrix4f::Identity();

    m(0, 3) = delta.x; mInv(0, 3) =-delta.x;
    m(1, 3) = delta.y; mInv(1, 3) =-delta.y;
    m(2, 3) = delta.z; mInv(2, 3) =-delta.z;

    return {m, mInv};
}

Transform Scale(float x, float y, float z) {
    Eigen::Matrix4f m    = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f mInv = Eigen::Matrix4f::Identity();

    m(0, 0) = x; mInv(0, 0) = 1.0f/x;
    m(1, 1) = y; mInv(1, 1) = 1.0f/y;
    m(2, 2) = z; mInv(2, 2) = 1.0f/z;

    return {m, mInv};
}

Transform RotateX(float theta) {
    float sinTheta = std::sin(Radians(theta));
    float cosTheta = std::cos(Radians(theta));

    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m(1, 1) = cosTheta; m(1, 2) =-sinTheta;
    m(2, 1) = sinTheta; m(2, 2) =-cosTheta;

    return {m, m.transpose()};
}

Transform RotateY(float theta) {
    float sinTheta = std::sin(Radians(theta));
    float cosTheta = std::cos(Radians(theta));

    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m(0, 0) = cosTheta; m(0, 2) = sinTheta;
    m(2, 0) =-sinTheta; m(2, 2) = cosTheta;

    return {m, m.transpose()};
}

Transform RotateZ(float theta) {
    float sinTheta = std::sin(Radians(theta));
    float cosTheta = std::cos(Radians(theta));

    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m(0, 0) = cosTheta; m(0, 1) =-sinTheta;
    m(1, 0) = sinTheta; m(1, 1) = cosTheta;

    return {m, m.transpose()};
}

Transform AxisAngle(float theta, const Vector3f &axis) {
    // Eigen::Vector3f axis_normalized(axis.x, axis.y, axis.z);
    Eigen::Vector3f axis_normalized = PBRender::Normalize(axis).ToEigen();
    Eigen::AngleAxisf R(theta, axis_normalized);

    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m.topLeftCorner<3, 3>() = R.toRotationMatrix();

    return {m, m.transpose()};
}

Transform LookAt(const Point3f &pos, const Point3f &look, const Vector3f &up) {
    Eigen::Matrix4f cameraToWorld = Eigen::Matrix4f::Identity();

    // Initialize the last column of viewing matrix
    cameraToWorld(0, 3) = pos.x;
    cameraToWorld(1, 3) = pos.y;
    cameraToWorld(2, 3) = pos.z;

    // Initialize the first three columns of viewing matrix
    Eigen::Vector3f Elook = look.ToEigen();
    Eigen::Vector3f Epos  = pos.ToEigen();
    Eigen::Vector3f Eup   = up.ToEigen().normalized();
    Eigen::Vector3f dir   = (Elook - Epos).normalized();

    if (Eup.cross(dir).norm() == 0) {
        throw "UP vector and viewing direction passed to LookAt are pointing in the same direction. Using the identity transformation.";
        return Transform{};
    }

    Eigen::Vector3f right = Eup.cross(dir).normalized();
    Eigen::Vector3f newUp = dir.cross(right).normalized();

    cameraToWorld(0, 0) = right(0);
    cameraToWorld(1, 0) = right(1);
    cameraToWorld(2, 0) = right(2);
    cameraToWorld(3, 0) = 0.f;
    cameraToWorld(0, 1) = newUp(0);
    cameraToWorld(1, 1) = newUp(1);
    cameraToWorld(2, 1) = newUp(2);
    cameraToWorld(3, 1) = 0.f;
    cameraToWorld(0, 2) = dir(0);
    cameraToWorld(1, 2) = dir(1);
    cameraToWorld(2, 2) = dir(2);
    cameraToWorld(3, 2) = 0.f;

    return Transform(cameraToWorld.inverse(), cameraToWorld);
}

Transform Orthographic(float zNear, float zFar) {
    return Scale(1, 1, 1 / (zFar - zNear)) * Translate(Vector3f(0, 0, -zNear));
}

Transform Perspective(float fov, float n, float f) {
    Eigen::Matrix4f persp;
    persp << 1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, f / (f - n), -f*n / (f - n),
             0, 0, 1, 0;
    
    // Scale canonical perspective view to specified field of view
    float invTanAng = 1 / std::tan(Radians(fov) / 2);

    return Scale(invTanAng, invTanAng, 1) * Transform(persp);
}

} // namespace PBRender