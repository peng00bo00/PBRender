#pragma once

#include "PBRender.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "transform.h"
#include "primitive.h"
#include "shapes/triangle.h"

namespace PBRender {

class ModelLoader {
    public:
        void loadModel(std::string path, const Transform &ObjectToWorld);
        void processNode(aiNode *node, const aiScene *scene, const Transform &ObjectToWorld);
        std::shared_ptr<TriangleMesh> processMesh(aiMesh *mesh, const aiScene *scene, const Transform &ObjectToWorld);

        void buildNoTextureModel(Transform &Object2World, 
                                 std::vector<std::shared_ptr<Primitive>> &prims,
                                 std::shared_ptr<Material> material);

    public:
        std::vector<std::shared_ptr<TriangleMesh>> meshes;
        std::string directory;
};


}