#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"
#include <functional>
#include <unordered_map>

struct VertexIndex {
    int v, vt, vn;
    uint32_t colorRGB;
    
    VertexIndex(int _v, int _vt, int _vn, const sglm::vec3& _color) : v(_v), vt(_vt), vn(_vn) {
        uint32_t R = static_cast<uint32_t>(_color.r * 255.0f);
        uint32_t G = static_cast<uint32_t>(_color.g * 255.0f);
        uint32_t B = static_cast<uint32_t>(_color.b * 255.0f);

        colorRGB = (R << 16) | (G << 8) | B;
    }

    VertexIndex(int _v, int _vt, int _vn, uint32_t _colorRGB) : v(_v), vt(_vt), vn(_vn), colorRGB(_colorRGB) {}

    bool operator==(const VertexIndex& other) const {
        if (v == other.v && vt == other.vt && vn == other.vn && colorRGB == other.colorRGB)
            return true;
        return false;
    }
};

struct VertexHash {
    size_t operator()(const VertexIndex& k) const{
        size_t h1 = std::hash<int>{}(k.v);
        size_t h2 = std::hash<int>{}(k.vt);
        size_t h3 = std::hash<int>{}(k.vn);
        size_t h4 = std::hash<uint32_t>{}(k.colorRGB);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

struct LoadInfo {
    std::vector<sglm::vec3> vertexInfo;
    std::vector<sglm::vec2> texCoordInfo;
    std::vector<sglm::vec2> generateTexCoordInfo;
    std::vector<sglm::vec3> normalInfo;
    std::vector<sglm::vec3> generateNormalInfo;
    std::unordered_map<VertexIndex, uint32_t, VertexHash> fmap;
};

CLASS_PTR(Model);
class Model {
public:
    static ModelUPtr Load(const std::string& filename);

    void Draw() const;

private:
    Model() {}
    bool LoadOBJFile(const std::string& filename);
    void ProcessVertexLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo);
    void ProcessTextureLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo);
    void ProcessNormalLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo);
    void ProcessFaceLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo);

    sglm::vec2 generateUV(const sglm::vec3& position, const sglm::vec3& normal);

    // VBO, EBO data
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    MeshPtr m_mesh;
};

#endif