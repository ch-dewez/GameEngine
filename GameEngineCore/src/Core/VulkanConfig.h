#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include <GLFW/glfw3.h>

namespace VulkanConfig {
// Instance validation layers
static const std::vector<const char*> ValidationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

// Device extensions
static const std::vector<const char*> DeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    //"VK_KHR_portability_subset"
};

// Debug settings
#ifdef NDEBUG
static const bool EnableValidationLayers = false;
#else
static const bool EnableValidationLayers = true;
#endif

// Helper functions
bool checkDeviceExtensionSupport(VkPhysicalDevice device);

static const bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : ValidationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}


static const std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (EnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
};
}
