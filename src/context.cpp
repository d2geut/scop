#include "context.h"
#include <iostream>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Context::Init() {
    float vertices[] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    // VAO Binding
    m_vertexLayout = VertexLayout::Create();
    // VBO Binding
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 12);

    // Vertex attribute setting
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // EBO Binding
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    std::cout << "vertex shader id: " << vertShader->Get() << std::endl;
    std::cout << "fragment shader id: " << fragShader->Get() << std::endl;

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    std::cout << "program id: " << m_program->Get() << std::endl;

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    return true;
}