#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include <vector>

struct Vertex {
    sglm::vec3 position;
    sglm::vec3 normal;
    sglm::vec2 texCoord;
    sglm::vec3 color;
};

CLASS_PTR(Mesh);
class Mesh {
public:
    static MeshUPtr Create(const std::vector<Vertex>& vertices, 
        const std::vector<uint32_t>& indices, uint32_t primitiveType);

    const VertexLayout* GetVertexLayout() const {
        return m_vertexLayout.get();
    }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }

    void Draw() const;

private:
    Mesh() {}
    void Init(
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        uint32_t primitiveType);

    uint32_t m_primitiveType { GL_TRIANGLES };
    VertexLayoutUPtr m_vertexLayout;
    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
};

#endif // __MESH_H__