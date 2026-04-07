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

    // space
    if (Input::IsKeyDown(GLFW_KEY_SPACE))
        m_animation ? (m_animation = false) : (m_animation = true);
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
    auto lightModelTransform = sglm::translate(sglm::mat4(1.0), m_light.position) * sglm::scale(sglm::mat4(1.0), sglm::vec3(0.1f));
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", sglm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
    m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
    m_box->Draw();

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", m_light.position);
    m_program->SetUniform("light.direction", m_light.direction);
    m_program->SetUniform("light.range", m_light.distance);
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);
    m_program->SetUniform("material.diffuse", 0);
    m_program->SetUniform("material.specular", 1);
    m_program->SetUniform("material.shininess", m_material.shininess);

    glActiveTexture(GL_TEXTURE0);
    m_material.diffuse->Bind();
    glActiveTexture(GL_TEXTURE1);
    m_material.specular->Bind();

    for (size_t i = 0; i < cubePositions.size(); i++) {
        auto& pos = cubePositions[i];   
        auto model = sglm::translate(sglm::mat4(1.0f), pos);
        auto angle = sglm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i);
        model = sglm::rotate(model, m_animation ? angle : 0.0f, sglm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        m_program->SetUniform("modelTransform", model);
        m_box->Draw();
    }
}

bool Context::Init() {
    m_box = Mesh::CreateBox();

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    m_material.diffuse = Texture::CreateFromImage(Image::Load("./image/container2.bmp").get());
    m_material.specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.bmp").get());

    return true;
}