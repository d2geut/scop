#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"
#include <functional>
#include <unordered_map>

struct VertexIndex {
    int v, vt, vn;
    
    bool operator==(const VertexIndex& other) const {
        if (v == other.v && vt == other.vt && vn == other.vn)
            return true;
        return false;
    }
};

struct VertexHash {
    size_t operator()(const VertexIndex& k) const{
        size_t h1 = std::hash<int>{}(k.v);
        size_t h2 = std::hash<int>{}(k.vt);
        size_t h3 = std::hash<int>{}(k.vn);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

CLASS_PTR(Model);
class Model {
public:
    static ModelUPtr Load(const std::string& filename);

    void Draw() const;

private:
    Model() {}
    bool LoadOBJFile(const std::string& filename);
    void ProcessVertexLine(std::stringstream& ss, const std::string& filename, size_t lineCount);
    void ProcessTextureLine(std::stringstream& ss, const std::string& filename, size_t lineCount);
    void ProcessNormalLine(std::stringstream& ss, const std::string& filename, size_t lineCount);
    void ProcessFaceLine(std::stringstream& ss, const std::string& filename, size_t lineCount);

    // VBO, EBO data
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // vertex, texcoord, normal
    std::vector<sglm::vec3> vertexInfo;
    std::vector<sglm::vec2> texCoordInfo;
    std::vector<sglm::vec3> normalInfo;

    std::unordered_map<VertexIndex, uint32_t, VertexHash> fmap;

    MeshPtr m_mesh;
};

#endif