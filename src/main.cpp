#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "common.h"
#include "shader.h"
#include "program.h"
#include "context.h"
#include "constants.h"
#include "input.h"

Input& g_input = Input::instance();

// window의 프레임버퍼 크기 변경
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    std::cout << "framebuffer size changed: (" << width << " x " << height << ")" << std::endl;
    glViewport(0, 0, width, height);
}

int main()
{
    std::cout << "Program Start!" << std::endl;

    // glfw 라이브러리 초기화
    std::cout << "Initialize glfw" << std::endl;
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        std::cerr << "failed to initialize glfw: " << description << std::endl;
        return -1;
    }

    // glfw 윈도우 생성
    std::cout << "Create glfw window" << std::endl;
    GLFWwindow* window = glfwCreateWindow(Constants::WindowWidth, Constants::WindowHeight, Constants::WindowName, nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to create glfw window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "failed to initialie glad" << std::endl;
        glfwTerminate();
        return -1;
    }
    const GLubyte* glVersion = glGetString(GL_VERSION);
    std::cout << "OpenGL context version: " << glVersion << std::endl;

    auto context = Context::Create();
    if (!context) {
        std::cerr << "failed to create context" << std::endl;
        glfwTerminate();
        return -1;
    }

    OnFramebufferSizeChange(window, Constants::WindowWidth, Constants::WindowHeight);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, Input::OnKeyEvent);
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE); // test 용

    // glfw loop
    std::cout << "Start main loop" << std::endl;

    float currentFrame, lastFrame, deltaTime = 0;
    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        context->ProcessInput(window, deltaTime);
        context->Render();
        glfwSwapBuffers(window);
    }
    context.reset(); // 스마트포인터 리셋 방법

    glfwTerminate();
    return 0;
}