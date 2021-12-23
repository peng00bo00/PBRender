#include "lights/infinite.h"
#include "sampling.h"

#include "ext/stbim/stb_image.h"

namespace PBRender {

void get_sphere_uv(const Vector3f &p, float &u, float &v) {
    float phi = atan2(p.z, p.x);
    if (phi < 0.f) phi += 2 * Pi;

    float theta = Pi - acos(p.y);

    u = phi * Inv2Pi;
    v = theta * InvPi;
}

Spectrum SkyBoxLight::Le(const Ray &r) const {

    Vector3f oc = r.o - worldCenter;

    float a = Dot(r.d, r.d);
    float b = 2.0 * Dot(oc, r.d);
    float c = Dot(oc, oc) - worldRadius * worldRadius;
    float discriminant = b*b - 4*a*c;
    float t;

    if (discriminant < 0) return 0.f;

    t = (-b + sqrt(discriminant)) / (2.0 * a);
    Point3f hitPos = r.o + t * r.d;
    Vector3f hitPos_temp = hitPos - worldCenter;

    if (data) {
        float u, v;
        get_sphere_uv(hitPos_temp, u, v);
        return getLightValue(u, v);
    }

    Spectrum Col;

    Col[0] = (hitPos_temp.x + worldRadius) / (2.f * worldRadius);
    Col[1] = (hitPos_temp.y + worldRadius) / (2.f * worldRadius);
    Col[2] = (hitPos_temp.z + worldRadius) / (2.f * worldRadius);

    return Col;
}

bool SkyBoxLight::loadImage(char *imageFile) {
    stbi_set_flip_vertically_on_load(true);

    data = stbi_loadf(imageFile, &imageWidth, &imageHeight, &nrComponents, 0);

    if (data) {
        std::cout << "Successfully load file: " << imageFile << "!" << std::endl;
        return true;
    }
    return false;
}

Spectrum SkyBoxLight::Sample_Li(const Interaction &ref, const Point2f &u,
                                Vector3f *wi, float *pdf,
                                VisibilityTester *vis) const {
    float theta = u.y * Pi;
    float phi = u.x * 2 * Pi;

    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);

    float sinPhi = std::sin(phi);
    float cosPhi = std::cos(phi);

    *wi = LightToWorld(Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));
    *pdf = 1.f / (4 * Pi);
    *vis = VisibilityTester(ref, Interaction(ref.p + *wi * (2 * worldRadius), ref.time));

    return getLightValue(u.x, u.y);
}

}