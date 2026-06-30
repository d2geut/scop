#include "context.h"
#include <iostream>
#include "input.h"

ContextUPtr Context::Create(const std::string& filename) {
    auto context = ContextUPtr(new Context());
    if (!context->Init(filename))
        return nullptr;
    return std::move(context);
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
        m_texmode ? (m_texmode = false) : (m_texmode = true);
}

void Context::MouseMove(double x, double y) {
    if (!m_cameraControl)
        return;

    auto pos = sglm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    if (m_cameraPosButton) {
        auto cameraFront = m_cameraTarget - m_cameraPos;
        auto cameraRight = sglm::normalize(sglm::cross(m_cameraUp, -cameraFront));
        auto cameraUp = sglm::normalize(sglm::cross(-cameraFront, cameraRight));

        const float cameraSpeed = m_cameraDistance * 0.001f;
        m_cameraTarget -= cameraRight * deltaPos.x * cameraSpeed;
        m_cameraTarget += cameraUp * deltaPos.y * cameraSpeed;
    }
    else {
        const float cameraRotSpeed = 0.2f;
        m_cameraYaw -= deltaPos.x * cameraRotSpeed;
        m_cameraPitch += deltaPos.y * cameraRotSpeed;

        if (m_cameraYaw < 0.0f)
            m_cameraYaw += 360.0f;
        if (m_cameraYaw > 360.0f)
            m_cameraYaw -= 360.0f;
        if (m_cameraPitch > 89.0f)
            m_cameraPitch = 89.0f;
        if (m_cameraPitch < -89.0f)
            m_cameraPitch = -89.0f;
    }
    m_cameraPos.x = m_cameraTarget.x + m_cameraDistance * cos(sglm::radians(m_cameraPitch)) * sin(sglm::radians(m_cameraYaw));
    m_cameraPos.y = m_cameraTarget.y + m_cameraDistance * sin(sglm::radians(m_cameraPitch));
    m_cameraPos.z = m_cameraTarget.z + m_cameraDistance * cos(sglm::radians(m_cameraPitch)) * cos(sglm::radians(m_cameraYaw));

    m_prevMousePos = pos;
}

void Context::MouseScroll(double x, double y) {
    const float cameraSpeed = 0.1f;
    m_cameraDistance -= y * cameraSpeed;

    if (m_cameraDistance < 0.1f)
        m_cameraDistance = 0.1f;
    
    m_cameraPos.x = m_cameraTarget.x + m_cameraDistance * cos(sglm::radians(m_cameraPitch)) * sin(sglm::radians(m_cameraYaw));
    m_cameraPos.y = m_cameraTarget.y + m_cameraDistance * sin(sglm::radians(m_cameraPitch));
    m_cameraPos.z = m_cameraTarget.z + m_cameraDistance * cos(sglm::radians(m_cameraPitch)) * cos(sglm::radians(m_cameraYaw));
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            m_prevMousePos = sglm::vec2((float)x, (float)y);
            m_cameraControl = true;
            if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
                m_cameraPosButton = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
            m_cameraPosButton = false;
        }
    }
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    auto projection = sglm::perspective(sglm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 20.0f);
    auto view = sglm::lookAt(m_cameraPos, m_cameraTarget, m_cameraUp);

    // texmode on
    if (m_texmode) {
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
        m_material.texdiffuse->Bind();
        glActiveTexture(GL_TEXTURE1);
        m_material.texspecular->Bind();

        auto pos = sglm::vec3();
        auto model = sglm::translate(sglm::mat4(1.0f), pos);
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        m_program->SetUniform("modelTransform", model);
        m_model->Draw();
    }
    else {
        m_program2->Use();
        m_program2->SetUniform("viewPos", m_cameraPos);
        m_program2->SetUniform("light.position", m_light.position);
        m_program2->SetUniform("light.direction", m_light.direction);
        m_program2->SetUniform("light.range", m_light.distance);
        m_program2->SetUniform("light.ambient", m_light.ambient);
        m_program2->SetUniform("light.diffuse", m_light.diffuse);
        m_program2->SetUniform("light.specular", m_light.specular);
        m_program2->SetUniform("material.specular", m_material.specular);
        m_program2->SetUniform("material.shininess", m_material.shininess);

        auto pos = sglm::vec3();
        auto model = sglm::translate(sglm::mat4(1.0f), pos);
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        m_program->SetUniform("modelTransform", model);
        m_model->Draw();
    }
}

bool Context::Init(const std::string& filename) {
    m_model = Model::Load(filename);
    if (!m_model)
        return false;

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;

    m_program2 = Program::Create("./shader/color_mode.vs", "./shader/color_mode.fs");
    if (!m_program2)
        return false;

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    auto diffImage = Image::Load("./image/container2.bmp");
    auto specImage = Image::Load("./image/container2_specular.bmp");
    if (!diffImage || !specImage)
        return false;

    m_material.texdiffuse = Texture::CreateFromImage(diffImage.get());
    m_material.texspecular = Texture::CreateFromImage(specImage.get());

    return true;
}