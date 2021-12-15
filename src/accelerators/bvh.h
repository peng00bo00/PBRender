#pragma once

#include "PBRender.h"
#include "primitive.h"

namespace PBRender {

struct BVHBuildNode;

struct BVHPrimitiveInfo;
struct MortonPrimitive;
struct LinearBVHNode;

// save for future changes
// class BVHAccel : public Aggregate {
class BVHAccel {
    public:
        BVHAccel(std::vector<std::shared_ptr<Primitive>> p,
                 int maxPrimsInNode = 1);
        
        ~BVHAccel();

        Bounds3f WorldBound() const;
        bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
        bool IntersectP(const Ray &ray) const;
    
    private:
        BVHBuildNode *recursiveBuild(
            std::vector<BVHPrimitiveInfo> &primitiveInfo,
            int start, int end, int *totalNodes,
            std::vector<std::shared_ptr<Primitive>> &orderedPrims);
        int flattenBVHTree(BVHBuildNode *node, int *offset);
    
    private:
        const int maxPrimsInNode;
        std::vector<std::shared_ptr<Primitive>> primitives;
        LinearBVHNode *nodes = nullptr;
};

std::shared_ptr<BVHAccel> CreateBVHAccelerator(std::vector<std::shared_ptr<Primitive>> prims);

}