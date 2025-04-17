#pragma once
#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Engine {
    
class Input {
public:
    static void Init(GLFWwindow* window);
    static Input& Instance();

    Input(GLFWwindow* window);

    void removeAllCallbacks();

    void setCursorMode(int value); 
    void setInputMode(int mode, int value);

    void addOnKeyPressed(std::function<void(int)> callback);
    void addOnKeyReleased(std::function<void(int)> callback);
    bool isKeyPressed(int key);

    glm::vec2 getMouseDelta();
    glm::vec2 getMousePosition();
private:
    std::vector<std::function<void(int)>> m_keyPressedCallback;
    std::vector<std::function<void(int)>> m_keyReleasedCallback;

    glm::vec2 mousePos;
    glm::vec2 lastMousePos;

    GLFWwindow* m_window;
    static void glfwKeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwMouseCallback(GLFWwindow* window, double xpos, double ypos);
};

}


