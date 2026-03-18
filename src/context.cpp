#include "context.h"
#include <iostream>
#include "input.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::ProcessInput(GLFWwindow* window, float deltaTime) {
    const float cameraSpeed = 2.0f * deltaTime;

    if (Input::IsKeyDown(GLFW_KEY_W))
        m_cameraPos += m_cameraFront * cameraSpeed;
    if (Input::IsKeyDown(GLFW_KEY_S))
        m_cameraPos -= m_cameraFront * cameraSpeed;

    auto cameraRight = sglm::normalize(sglm::cross(m_cameraUp, -m_cameraFront));
    if (Input::IsKeyDown(GLFW_KEY_D)) {
        m_cameraPos += cameraRight * cameraSpeed;
    }
    if (Input::IsKeyDown(GLFW_KEY_A))
        m_cameraPos -= cameraRight * cameraSpeed;    

    auto cameraUp = sglm::normalize(sglm::cross(-m_cameraFront, cameraRight));
    if (Input::IsKeyDown(GLFW_KEY_E))
        m_cameraPos += cameraUp * cameraSpeed;
    if (Input::IsKeyDown(GLFW_KEY_Q))
        m_cameraPos -= cameraUp * cameraSpeed;
}

void Context::MouseMove(double x, double y) {
    if (!m_cameraControl)
        return;
    auto pos = sglm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.5f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f)
        m_cameraYaw -= 360.0f;
    if (m_cameraPitch > 89.0f)
        m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f)
        m_cameraPitch = -89.0f;
    
    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            // 마우스 조작 시작 시점에 현 마우스 커서 위치 저장
            m_prevMousePos = sglm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

void Context::Render() {
    std::vector<sglm::vec3> cubePositions = {
        sglm::vec3(0.0f, 0.0f, 0.0f),
        sglm::vec3(2.0f, 5.0f, -15.0f),
        sglm::vec3(-1.5f, -2.2f, -2.5f),
        sglm::vec3(-3.8f, -2.0f, -12.3f),
        sglm::vec3(2.4f, -0.4f, -3.5f),
        sglm::vec3(-1.7f, 3.0f, -7.5f),
        sglm::vec3(1.3f, -2.0f, -2.5f),
        sglm::vec3(1.5f, 2.0f, -2.5f),
        sglm::vec3(1.5f, 0.2f, -1.5f),
        sglm::vec3(-1.3f, 1.0f, -1.5f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    sglm::vec4 cameraFront = sglm::rotate(sglm::mat4(1.0f), sglm::radians(m_cameraYaw), sglm::vec3(0.0f, 1.0f, 0.0f)) * sglm::rotate(sglm::mat4(1.0f), sglm::radians(m_cameraPitch), sglm::vec3(1.0f, 0.0f, 0.0f)) * sglm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    m_cameraFront = sglm::vec3(cameraFront.x, cameraFront.y, cameraFront.z);
    auto projection = sglm::perspective(sglm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 20.0f);
    auto view = sglm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    // light cube
    auto lightModelTransform = sglm::translate(sglm::mat4(1.0), m_lightPos) * sglm::scale(sglm::mat4(1.0), sglm::vec3(0.1f));
    m_program->Use();
    m_program->SetUniform("lightPos", m_lightPos);
    m_program->SetUniform("lightColor", sglm::vec3(1.0f, 1.0f, 1.0f));
    m_program->SetUniform("objectColor", sglm::vec3(1.0f, 1.0f, 1.0f));
    m_program->SetUniform("ambientStrength", 1.0f);
    m_program->SetUniform("transform", projection * view * lightModelTransform);
    m_program->SetUniform("modelTransform", lightModelTransform);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("lightPos", m_lightPos);
    m_program->SetUniform("lightColor", m_lightColor);
    m_program->SetUniform("objectColor", m_objectColor);
    m_program->SetUniform("ambientStrength", m_ambientStrength);
    m_program->SetUniform("specularStrength", m_specularStrength);
    m_program->SetUniform("specularShininess", m_specularShininess);

    for (size_t i = 0; i < cubePositions.size(); i++) {
        auto& pos = cubePositions[i];   
        auto model = sglm::translate(sglm::mat4(1.0f), pos);
        auto angle = sglm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i);
        model = sglm::rotate(model, m_animation ? angle : 0.0f, sglm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        m_program->SetUniform("modelTransform", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

bool Context::Init() {
    float vertices[] = { // pos.xyz, normal.xyz, texcoord.uv
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    // VAO Binding
    m_vertexLayout = VertexLayout::Create();
    // VBO Binding
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 8 * 6 * 4);

    // Vertex attribute setting
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);

    // EBO Binding
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/lighting.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/lighting.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    std::cout << "vertex shader id: " << vertShader->Get() << std::endl;
    std::cout << "fragment shader id: " << fragShader->Get() << std::endl;

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    std::cout << "program id: " << m_program->Get() << std::endl;

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    auto image = Image::Load("./image/container.bmp");
    if (!image)
        return false;
    std::cout << "image: " << image->GetWidth() << "x" << image->GetHeight() << ", " << image->GetChannelCount() << " channels" << std::endl;

    // Texture Generate
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./image/awesomeface.bmp");
    if (!image2)
        return false;
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    return true;
}