#include <iostream>
#include "GLFW/glfw3.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_NAME "Hello, OpenGL!"

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
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to create glfw window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // glfw loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}