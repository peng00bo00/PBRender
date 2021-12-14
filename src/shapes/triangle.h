#pragma once

#include "PBRender.h"
#include "shape.h"

namespace PBRender {

struct TriangleMesh {
    // TriangleMesh Public Methods
    TriangleMesh(const Transform &ObjectToWorld, int nTriangles,
                 const int *vertexIndices, int nVertices, const Point3f *P,
                 const Vector3f *S, const Normal3f *N, const Point2f *uv,
                //  const std::shared_ptr<Texture<float>> &alphaMask,
                //  const std::shared_ptr<Texture<float>> &shadowAlphaMask,
                 const int *faceIndices);

    // TriangleMesh Data
    const int nTriangles, nVertices;
    std::vector<int> vertexIndices;
    std::unique_ptr<Point3f[]> p;
    std::unique_ptr<Normal3f[]> n;
    std::unique_ptr<Vector3f[]> s;
    std::unique_ptr<Point2f[]> uv;
    // std::shared_ptr<Texture<float>> alphaMask, shadowAlphaMask;
    std::vector<int> faceIndices;
};

static long long triMeshBytes = 0;

class Triangle : public Shape {
    public:
        // Triangle Public Methods
        Triangle(const Transform *ObjectToWorld, const Transform *WorldToObject,
                bool reverseOrientation, const std::shared_ptr<TriangleMesh> &mesh,
                int triNumber)
            : Shape(ObjectToWorld, WorldToObject, reverseOrientation), mesh(mesh) {
            v = &mesh->vertexIndices[3 * triNumber];
            triMeshBytes += sizeof(*this);
            faceIndex = mesh->faceIndices.size() ? mesh->faceIndices[triNumber] : 0;
        }
        Bounds3f ObjectBound() const;
        Bounds3f WorldBound() const;
        bool Intersect(const Ray &ray, float *tHit, SurfaceInteraction *isect,
                       bool testAlphaTexture = true) const;
        bool IntersectP(const Ray &ray, bool testAlphaTexture = true) const;
        float Area() const;

        // using Shape::Sample;  // Bring in the other Sample() overload.
        // Interaction Sample(const Point2f &u, float *pdf) const;

        // // Returns the solid angle subtended by the triangle w.r.t. the given
        // // reference point p.
        // float SolidAngle(const Point3f &p, int nSamples = 0) const;

    private:
        // Triangle Private Methods
        void GetUVs(Point2f uv[3]) const {
            if (mesh->uv) {
                uv[0] = mesh->uv[v[0]];
                uv[1] = mesh->uv[v[1]];
                uv[2] = mesh->uv[v[2]];
            } else {
                uv[0] = Point2f(0, 0);
                uv[1] = Point2f(1, 0);
                uv[2] = Point2f(1, 1);
            }
        }

        // Triangle Private Data
        std::shared_ptr<TriangleMesh> mesh;
        const int *v;
        int faceIndex;
};

}