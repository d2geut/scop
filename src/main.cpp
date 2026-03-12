#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "common.h"
#include "shader.h"
#include "program.h"
#include "context.h"
#include "constants.h"

// window의 프레임버퍼 크기 변경
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    std::cout << "framebuffer size changed: (" << width << " x " << height << ")" << std::endl;
    glViewport(0, 0, width, height);
}

// 키보드 입력 콜백
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::cout << "key: " << key \
        << ", scancode: " << scancode \
        << ", action: " << (action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown") \
        << ", mods: " << (mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-") << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
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
    glfwSetKeyCallback(window, OnKeyEvent);

    // glfw loop
    std::cout << "Start main loop" << std::endl;
    while (!glfwWindowShouldClose(window)) {
        context->Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    context.reset(); // 스마트포인터 리셋 방법

    glfwTerminate();
    return 0;
}