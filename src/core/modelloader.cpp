#include "modelloader.h"

namespace PBRender {

void ModelLoader::processNode(aiNode *node, const aiScene *scene, const Transform &ObjectToWorld) {
    for (size_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, ObjectToWorld));
    }

    for (size_t i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene, ObjectToWorld);
    }
}

void ModelLoader::loadModel(std::string path, const Transform &ObjectToWorld) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Cannot load model!" << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene, ObjectToWorld);
}

std::shared_ptr<TriangleMesh> ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene, const Transform &ObjectToWorld) {
    size_t nVertices = mesh->mNumVertices;
    size_t nTriangles= mesh->mNumFaces;

    int *vertexIndices = new int[nTriangles * 3];
    Point3f *P = new Point3f[nVertices];
    Vector3f *S = nullptr; // new Vector3f[nVertices];
    Normal3f *N = new Normal3f[nVertices];
    Point2f *uv = new Point2f[nVertices];
    int *faceIndices = nullptr;

    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        P[i].x = mesh->mVertices[i].x;
        P[i].y = mesh->mVertices[i].y;
        P[i].z = mesh->mVertices[i].z;

        if (mesh->HasNormals()) {
            N[i].x = mesh->mNormals[i].x;
            N[i].y = mesh->mNormals[i].y;
            N[i].z = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0]) {
            uv[i].x = mesh->mTextureCoords[0][i].x;
            uv[i].y = mesh->mTextureCoords[0][i].y;
        }
    }

    for (size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];

        for (size_t j = 0; j < face.mNumIndices; ++j)
        {
            vertexIndices[3*i+j] = face.mIndices[j];
        }
    }
    
    if (!mesh->HasNormals()) {
        delete[] N;
        N = nullptr;
    }

    if (!mesh->mTextureCoords[0]) {
        delete[] uv;
        uv = nullptr;
    }

    std::shared_ptr<TriangleMesh> tm = std::make_shared<TriangleMesh>(
        ObjectToWorld, nTriangles, vertexIndices, nVertices,
        P, S, N, uv, faceIndices
    );

    delete[] P;
    delete[] S;
    delete[] N;
    delete[] uv;

    delete[] vertexIndices;

    return tm;
}

void ModelLoader::buildNoTextureModel(
        Transform &Object2World, 
        std::vector<std::shared_ptr<Primitive>> &prims,
        std::shared_ptr<Material> material) {
    std::vector<std::shared_ptr<Shape>> trisObj;
    Transform World2Object = Inverse(Object2World);
    int shapeID = 0;

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        for (size_t j = 0; j < meshes[i]->nTriangles; ++j)
        {
            std::shared_ptr<TriangleMesh> meshPtr = meshes[i];
            trisObj.push_back(std::make_shared<Triangle>(
                &Object2World, &World2Object, false, meshPtr, shapeID++
            ));
        }
    }
    std::cout << "Find " << shapeID << " triangles." << std::endl;

    for (size_t i = 0; i<trisObj.size() ; ++i) {
        prims.push_back(std::make_shared<GeometricPrimitive>(trisObj[i], material, nullptr));
    }
}


}