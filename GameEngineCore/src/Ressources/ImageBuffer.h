#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <memory>

namespace Engine {
namespace Ressources {

class ImageBuffer {
public:
    ImageBuffer(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );
    ~ImageBuffer();

    // Getters
    VkImage& getImage() { return m_image; }
    VkDeviceMemory& getImageMemory() { return m_imageMemory; }
    VkImageView& getImageView() { return m_imageView; }
    
    // Create image view with specific aspects
    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);

    // Transition image layout
    void transitionImageLayout(
        VkCommandPool* commandPool,
        VkQueue* graphicsQueue,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout
    );

    static VkFormat findDepthFormat();
    static bool hasStencilComponent(VkFormat format);
    static VkFormat findSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features
    );

private:
    void createImage();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    uint32_t m_width;
    uint32_t m_height;
    VkFormat m_format;
    VkImageTiling m_tiling;
    VkImageUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;

    VkImage m_image;
    VkDeviceMemory m_imageMemory;
    VkImageView m_imageView;
};

}
}
