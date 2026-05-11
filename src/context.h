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
#include "mesh.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window, float deltaTime);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_program2;
    ProgramUPtr m_simpleProgram;

    MeshUPtr m_box;

    // window size
    int m_width { Constants::WindowWidth };
    int m_height { Constants::WindowHeight };

    // texture - space·Î Á¦¾î
    bool m_texmode { false };

    // clear color
    sglm::vec4 m_clearColor { sglm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // light parameter
    struct Light {
        sglm::vec3 position { sglm::vec3(2.0f, 2.0f, 2.0f) };
        sglm::vec3 direction { sglm::vec3(-1.0f, -1.0f, -1.0f) };
        float distance { 50.0f };
        sglm::vec3 ambient { sglm::vec3(0.2f, 0.2f, 0.2f) };
        sglm::vec3 diffuse { sglm::vec3(0.5f, 0.5f, 0.5f) };
        sglm::vec3 specular { sglm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;

    // material parameter
    struct Material {
        TextureUPtr texdiffuse;
        TextureUPtr texspecular;
        sglm::vec3 ambient { sglm::vec3(1.0f, 0.5f, 0.31f) };
        sglm::vec3 diffuse { sglm::vec3(1.0f, 0.5f, 0.31f) };
        sglm::vec3 specular { sglm::vec3(0.5f, 0.5f, 0.5f) };
        float shininess { 32.0f };
    };
    Material m_material;

    // camera parameter
    bool m_cameraControl { false };
    sglm::vec2 m_prevMousePos { sglm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };
    sglm::vec3 m_cameraPos { sglm::vec3(0.0f, 0.0f, 3.0f) };
    sglm::vec3 m_cameraFront { sglm::vec3(0.0f, 0.0f, -1.0f) };
    sglm::vec3 m_cameraUp { sglm::vec3(0.0f, 1.0f, 0.0f) };
};

#endif