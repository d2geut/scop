#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "common.h"
#include "shader.h"
#include "program.h"
#include "context.h"
#include "constants.h"
#include "input.h"
#include "model.h"

Input& g_input = Input::instance();

// window의 프레임버퍼 크기 변경
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    std::cout << "framebuffer size changed: (" << width << " x " << height << ")" << std::endl;
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->Reshape(width, height);
}

void OnCursorPos(GLFWwindow* window, double x, double y) {
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnScrollEvent(GLFWwindow* window, double x, double y) {
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseScroll(x, y);
}


int main(int argc, char **argv)
{
    std::cout << "Program Start!" << std::endl;

    if (argc != 2) {
        std::cerr << "format: ./scop filename.obj" << std::endl;
        return 0;
    }

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

    auto context = Context::Create(argv[1]);
    if (!context) {
        std::cerr << "failed to create context" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, context.get()); // user pointer를 통해 glfw callback에 context 전달

    OnFramebufferSizeChange(window, Constants::WindowWidth, Constants::WindowHeight);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, Input::OnKeyEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScrollEvent);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        context->ProcessInput(window);
        context->Render();
        glfwSwapBuffers(window);
    }
    context.reset(); // 스마트포인터 리셋 방법

    glfwTerminate();
    return 0;
}