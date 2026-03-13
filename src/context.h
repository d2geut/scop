#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "image.h"
#include "texture.h"
#include "./sglm/sglm.h"
#include "constants.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window, float deltaTime);
    void Reshape(int width, int height);
private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;

    // window size
    int m_width { Constants::WindowWidth };
    int m_height { Constants::WindowHeight };

    // camera parameter
    sglm::vec3 m_cameraPos { sglm::vec3(0.0f, 0.0f, 3.0f) };
    sglm::vec3 m_cameraFront { sglm::vec3(0.0f, 0.0f, -1.0f) };
    sglm::vec3 m_cameraUp { sglm::vec3(0.0f, 1.0f, 0.0f) };
};

#endif