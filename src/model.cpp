#include "model.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

ModelUPtr Model::Load(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    if (!model->LoadOBJFile(filename))
        return nullptr;
    return std::move(model);
}

void Model::Draw() const {
    m_mesh->Draw();
}

void Model::ProcessVertexLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo) {
    sglm::vec3 temp;
    if (!(ss >> temp.x >> temp.y >> temp.z)) {
        std::cerr << filename << ": invalid vertex data [line: " << lineCount << "]" << std::endl;
        return;
    }
    linfo.vertexInfo.push_back(temp);
}

void Model::ProcessTextureLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo) {
    sglm::vec2 temp;
    if (!(ss >> temp.x >> temp.y)) {
        std::cerr << filename << ": invalid texcoord data [line: " << lineCount << "]" << std::endl;
        return;
    }
    linfo.texCoordInfo.push_back(temp);
}

void Model::ProcessNormalLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo) {
    sglm::vec3 temp;
    if (!(ss >> temp.x >> temp.y >> temp.z)) {
        std::cerr << filename << ": invalid normal data [line: " << lineCount << "]" << std::endl;
        return;
    }
    linfo.normalInfo.push_back(temp);
}

void Model::ProcessFaceLine(std::stringstream& ss, const std::string& filename, size_t lineCount, LoadInfo& linfo) {
    std::vector<std::string> faceInfo;
    // 분리안된 face indices를 담는 vector
    std::vector<uint32_t> findices;
    std::vector<VertexIndex> vidx;

    std::string info;
    while (ss >> info) {
        faceInfo.push_back(info);
    }

    size_t fsize = faceInfo.size();
    bool normalflag = false;
    if (fsize < 3) {
        std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
        return;
    }
    for (int i = 0; i < fsize; i++) {
        std::stringstream ss1(faceInfo[i]);
        std::string idx;
        vidx.push_back({0, 0, 0});

        int j = 0;
        while (std::getline(ss1, idx, '/')) {
            if (!idx.empty()) {
                try {
                    int index = std::stoi(idx);
                    if (j == 0) {
                        vidx[i].v = index;
                        if (index > linfo.vertexInfo.size() || index <= 0)
                            throw std::out_of_range("out of vertex info range");
                    }
                    else if (j == 1) {
                        vidx[i].vt = index;
                        if (index > linfo.texCoordInfo.size() || index < 0)
                            throw std::out_of_range("out of texcoord info range");
                    }
                    else if (j == 2) {
                        vidx[i].vn = index;
                        if (index > linfo.normalInfo.size() || index < 0)
                            throw std::out_of_range("out of normal info range");
                    }
                    else break ;
                }
                catch(const std::exception& e) {
                    std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
                    return;
                }
            }
            j++;
        }
        if (!vidx[i].vn)
            normalflag = true;
    }
    // normal이 0인 vertex가 존재
    if (normalflag) {
        sglm::vec3 gnormal = sglm::cross(linfo.vertexInfo[vidx[1].v - 1] - linfo.vertexInfo[vidx[0].v - 1], 
            linfo.vertexInfo[vidx[2].v - 1] - linfo.vertexInfo[vidx[0].v - 1]);
        linfo.generateNormalInfo.push_back(gnormal);
        int vnidx = linfo.normalInfo.size() + linfo.generateNormalInfo.size();
        for (int i = 0; i < fsize; i++) {
            vidx[i].vn = vnidx;
        }
    }

    // hash에 넣고 key값을 통해서 index return
    for (int i = 0; i < fsize; i++) {
        VertexIndex key = {vidx[i].v, vidx[i].vt, vidx[i].vn};
        auto it = linfo.fmap.find(key);
        if (it != linfo.fmap.end()) {
            findices.push_back(it->second);
        }
        else {
            Vertex vertice;

            // vertex 범위 검사
            if (vidx[i].v > linfo.vertexInfo.size() || vidx[i].v <= 0) {
                std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
                return;
            }
            vertice.position = linfo.vertexInfo[vidx[i].v - 1];

            // texCoord 범위 검사
            if (vidx[i].vt > 0) // vt가 있는 경우
                vertice.texCoord = linfo.texCoordInfo[vidx[i].vt - 1];
            else // vt가 없는 경우
                vertice.texCoord = sglm::vec2(0.0f, 0.0f);
            
            // normal 범위 검사
            if (vidx[i].vn > linfo.normalInfo.size()) // vn이 generatenormal인 경우
                vertice.normal = linfo.generateNormalInfo[vidx[i].vn - linfo.normalInfo.size() - 1];
            else
                vertice.normal = linfo.normalInfo[vidx[i].vn - 1];

            uint32_t fIdx = (uint32_t)vertices.size();
            vertices.push_back(vertice);
            findices.push_back(fIdx);

            linfo.fmap[key] = fIdx;
        }
    }
    // 면을 삼각형으로 분리
    int numTriangle = findices.size() - 2;
    for (int j = 0; j < numTriangle; j++) {
        indices.push_back(findices[0]);
        indices.push_back(findices[j + 1]);
        indices.push_back(findices[j + 2]);
    }
}

bool Model::LoadOBJFile(const std::string& filename) {
    LoadInfo linfo;

    // file format test
    if (!checkFileFormat(filename, ".obj")) {
        std::cerr << "failed to open OBJ file: " << filename << " is not OBJ file" << std::endl;
        return false;
    }

    // file open & read line
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "failed to open file: " << filename << std::endl;
        return false;
    }
    std::string line;
    size_t lineCount = 0;
    while (std::getline(fin, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        lineCount++;

        if (prefix == "v") {
            ProcessVertexLine(ss, filename, lineCount, linfo);
        }
        else if (prefix == "vt") {
            ProcessTextureLine(ss, filename, lineCount, linfo);
        }
        else if (prefix == "vn") {
            ProcessNormalLine(ss, filename, lineCount, linfo);
        }
        else if (prefix == "f") {
            ProcessFaceLine(ss, filename, lineCount, linfo);
        }
        else if (prefix == "#" || prefix == "")
            continue;
        else {
            std::cout << filename << ": unknown tag: " << prefix << std::endl;
            continue;
        }
    }

    // test code
    // std::cout << "[vertices]" << std::endl;
    // for (std::vector<Vertex>::iterator iter1 = vertices.begin(); iter1 != vertices.end(); iter1++) {
    //     sglm::vec3 position = iter1->position;
    //     sglm::vec2 texCoord = iter1->texCoord;
    //     sglm::vec3 normal = iter1->normal;
    //     std::cout << "(" << position.x << "," << position.y << "," << position.z << "), (" <<
    //         texCoord.x << "," << texCoord.y << "), (" <<
    //         normal.x << "," << normal.y << "," << normal.z << ")" << std::endl;
    // }
    // std::cout << "[indices]" << std::endl;
    // for (std::vector<uint32_t>::iterator iter2 = indices.begin(); iter2 != indices.end(); iter2++) {
    //     std::cout << *iter2 << " ";
    // }
    // std::cout << std::endl;

    m_mesh = Mesh::Create(vertices, indices, GL_TRIANGLES);

    return true;
}