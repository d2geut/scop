#include "input.h"
#include <iostream>

// 키보드 입력 콜백
void Input::OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= 1024)
        return;

    // std::cout << "key: " << key \
    //     << ", scancode: " << scancode \
    //     << ", action: " << (action == GLFW_PRESS ? "Pressed" :
    //     action == GLFW_RELEASE ? "Released" :
    //     action == GLFW_REPEAT ? "Repeat" : "Unknown") \
    //     << ", mods: " << (mods & GLFW_MOD_CONTROL ? "C" : "-",
    //     mods & GLFW_MOD_SHIFT ? "S" : "-",
    //     mods & GLFW_MOD_ALT ? "A" : "-") << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (action == GLFW_PRESS) {
        instance().m_keys[key].isPressed = true;
    }
    else if (action == GLFW_RELEASE) {
        instance().m_keys[key].isPressed = false;
    }
}

bool Input::IsKeyDown(int key) {
    return instance().m_keys[key].isPressed;
}