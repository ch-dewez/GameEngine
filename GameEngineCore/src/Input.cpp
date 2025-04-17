#include "Input.h"

namespace Engine {

static Input* g_inputInstance = nullptr;
void Input::Init(GLFWwindow* window) {
    g_inputInstance = new Input(window);
}
Input& Input::Instance() {
    return *g_inputInstance;
}

Input::Input(GLFWwindow* window)
: m_window(window)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetKeyCallback(window, glfwKeyEventCallback);
    glfwSetCursorPosCallback(window, glfwMouseCallback);
}

void Input::glfwKeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
//TODO: implement glfw key event callbakc (too lazy) 
};

void Input::glfwMouseCallback(GLFWwindow* window, double xpos, double ypos){
    auto& singleton = Input::Instance();
    singleton.lastMousePos = singleton.mousePos;
    singleton.mousePos = {xpos, ypos};
};

void Input::removeAllCallbacks() {
}

void Input::setCursorMode(int value) {
    glfwSetInputMode(m_window, GLFW_CURSOR, value);
}

void Input::setInputMode(int mode, int value) {
    glfwSetInputMode(m_window, mode, value);
}

void Input::addOnKeyPressed(std::function<void(int)> callback) {
    m_keyPressedCallback.push_back(callback);
}

void Input::addOnKeyReleased(std::function<void(int)> callback) {
    m_keyReleasedCallback.push_back(callback);
}

bool Input::isKeyPressed(int key){
    return glfwGetKey(m_window, key);
}


glm::vec2 Input::getMouseDelta(){
    return lastMousePos - mousePos;
};
glm::vec2 Input::getMousePosition(){
    return mousePos;
};

}
