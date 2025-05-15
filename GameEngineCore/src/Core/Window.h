#pragma once
#include <cstdint>
#include <functional>

#include <GLFW/glfw3.h>

class Window {
public:
    Window(const char* title, uint32_t width, uint32_t height);
    ~Window();

    void pollEvents();
    bool shouldClose();

    VkInstance getInstance() const { return m_instance; }
    VkSurfaceKHR getSurface() const { return m_surface; }
    GLFWwindow* getGLFWWindow() const { return m_window; }
    VkExtent2D getExtent() const { return {m_Width, m_Height}; }

    // Define callback type
    using ResizeCallback = std::function<void(int, int)>;

    void setResizeCallback(ResizeCallback callback) {
        m_resizeCallback = callback;
    }

private:
    void initWindow();
    void initVulkan();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        app->handleResize(width, height);
    }

    void handleResize(int width, int height) {
        if (m_resizeCallback) {
            m_resizeCallback(width, height);
        }
    }

    void createVulkanInstance();
    void setupVulkanDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void createSurface();

    void shutDownWindow();
    void shutDownVulkan();

private:
    //const int MAX_FRAMES_IN_FLIGHT = 2;


    GLFWwindow* m_window;

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_vulkanDebugMessenger;

    VkSurfaceKHR m_surface;


#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif


    const char* m_Title;
    uint32_t m_Width;
    uint32_t m_Height;

    ResizeCallback m_resizeCallback;
};
