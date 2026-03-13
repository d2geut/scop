#ifndef __INPUT_H__
#define __INPUT_H__

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class Input {
public:
    // singleton instance
    static Input& instance() {
        static Input inst;
        return inst;
    };
    static void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    static bool IsKeyDown(int key);

private:
    Input() {}
    Input(const Input& origin) {}
    Input& operator=(const Input& origin) { return *this; }
    ~Input() {}

    struct KeyState {
        bool isPressed = false;
        float lastActiveTime = 0.0f;
    };
    KeyState m_keys[1024];
    const float m_threshold = 0.1f;

    bool CheckKey(int key);
};

#endif