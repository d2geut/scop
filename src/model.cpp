#include "model.h"
#include <iostream>
#include <fstream>
#include <sstream>

ModelUPtr Model::Load(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    if (!model->LoadOBJFile(filename))
        return nullptr;
    return std::move(model);
}

void Model::ProcessVertexLine(std::stringstream& ss, const std::string& filename, size_t lineCount) {
    sglm::vec3 temp;
    if (!(ss >> temp.x >> temp.y >> temp.z)) {
        std::cerr << filename << ": invalid vertex data [line: " << lineCount << "]" << std::endl;
        return;
    }
    vertexInfo.push_back(temp);
}

void Model::ProcessTextureLine(std::stringstream& ss, const std::string& filename, size_t lineCount) {
    sglm::vec2 temp;
    if (!(ss >> temp.x >> temp.y)) {
        std::cerr << filename << ": invalid texcoord data [line: " << lineCount << "]" << std::endl;
        return;
    }
    texCoordInfo.push_back(temp);
}

void Model::ProcessNormalLine(std::stringstream& ss, const std::string& filename, size_t lineCount) {
    sglm::vec3 temp;
    if (!(ss >> temp.x >> temp.y >> temp.z)) {
        std::cerr << filename << ": invalid normal data [line: " << lineCount << "]" << std::endl;
        return;
    }
    normalInfo.push_back(temp);
}

void Model::ProcessFaceLine(std::stringstream& ss, const std::string& filename, size_t lineCount) {
    std::vector<std::string> faceInfo;
    std::string info;
    // 분리안된 face indices를 담는 vector
    std::vector<uint32_t> findices;

    while (ss >> info) {
        faceInfo.push_back(info);
    }
    if (faceInfo.size() < 3) {
        std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
        return;
    }

    std::vector<std::string>::iterator iter;
    for (std::vector<std::string>::iterator iter = faceInfo.begin(); iter != faceInfo.end(); iter++) {
        std::stringstream sss(*iter);
        std::string idx;
        int vIdx, vtIdx, vnIdx;
        vIdx = vtIdx = vnIdx = 0;
        int i = 0;
        while (std::getline(sss, idx, '/')) {
            if (!idx.empty()) {
                try {
                    int index = std::stoi(idx);
                    if (i == 0) vIdx = index;
                    else if (i == 1) vtIdx = index;
                    else if (i == 2) vnIdx = index;
                    else break ;
                }
                catch(const std::exception& e) {
                    std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
                    return;
                }            
            }
            i++;
        }

        // hash에 넣고 key값을 통해서 index return
        VertexIndex key = {vIdx, vtIdx, vnIdx};
        auto it = fmap.find(key);
        if (it != fmap.end()) {
            findices.push_back(it->second);
        }
        else {
            Vertex vertice;

            // vertex 범위 검사
            if (vIdx > vertexInfo.size() || vIdx <= 0) {
                std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
                return;
            }
            vertice.position = vertexInfo[vIdx - 1];

            // texCoord 범위 검사
            if (vtIdx > texCoordInfo.size()) {
                std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
                return;
            }
            else if (vtIdx > 0) // vt가 있는 경우
                vertice.texCoord = texCoordInfo[vtIdx - 1];
            else // vt가 없는 경우
                vertice.texCoord = sglm::vec2(0.0f, 0.0f);
            
            // normal 범위 검사
            if (vnIdx > normalInfo.size()) {
                std::cerr << filename << ": invalid face data [line: " << lineCount << "]" << std::endl;
                return;
            }
            else if (vnIdx > 0) // vn이 있는 경우
                vertice.normal = normalInfo[vnIdx - 1];
            else // vn이 없는 경우 임시 처리
                vertice.normal = sglm::vec3(0.0f, 0.0f, 0.0f);

            uint32_t fIdx = (uint32_t)vertices.size();
            vertices.push_back(vertice);
            findices.push_back(fIdx);

            fmap[key] = fIdx;
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
            ProcessVertexLine(ss, filename, lineCount);
        }
        else if (prefix == "vt") {
            ProcessTextureLine(ss, filename, lineCount);
        }
        else if (prefix == "vn") {
            ProcessNormalLine(ss, filename, lineCount);
        }
        else if (prefix == "f") {
            ProcessFaceLine(ss, filename, lineCount);
        }
        else if (prefix == "#" || prefix == "")
            continue;
        else {
            std::cout << filename << ": unknown tag: " << prefix << std::endl;
            continue;
        }
    }

    // test code
    std::cout << "[vertices]" << std::endl;
    for (std::vector<Vertex>::iterator iter1 = vertices.begin(); iter1 != vertices.end(); iter1++) {
        sglm::vec3 position = iter1->position;
        sglm::vec2 texCoord = iter1->texCoord;
        sglm::vec3 normal = iter1->normal;
        std::cout << "(" << position.x << "," << position.y << "," << position.z << "), (" <<
            texCoord.x << "," << texCoord.y << "), (" <<
            normal.x << "," << normal.y << "," << normal.z << ")" << std::endl;
    }
    std::cout << "[indices]" << std::endl;
    for (std::vector<uint32_t>::iterator iter2 = indices.begin(); iter2 != indices.end(); iter2++) {
        std::cout << *iter2 << " ";
    }
    std::cout << std::endl;

    m_mesh->Create(vertices, indices, GL_TRIANGLES);

    return true;
}