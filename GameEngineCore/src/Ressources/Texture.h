#include <string>
#include <GLFW/glfw3.h>
#include <vector>
#include "Renderer/VulkanApi.h"

namespace Engine {
namespace Ressources {

class Texture {
public:
    struct TextureCreateInfo {
        std::string path;
        int width;
        int height;
        std::vector<VkFormat> possibleFormats;
        VkFormatFeatureFlags featureFlags;
        VkImageAspectFlags aspectFlags;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags memoryProperties;

        static TextureCreateInfo getDefault(std::string path);
        static TextureCreateInfo getDefaultWihtoutPath(int width=0, int height=0);
        static TextureCreateInfo getDefaultForDepthBuffer(int width, int height);
    };
public:
    Texture(TextureCreateInfo& info);
    // This should not be used but VulkanApi need this class for initialization but this class need the singleton which is not yet iniatialized
    Texture(TextureCreateInfo& info, ::Engine::Renderer::VulkanApi* api);
    ~Texture();

    VkDescriptorImageInfo createDescriptorImageInfo();
    VkFormat getFormat() {return m_format;};
    VkImageView getImageView() {return m_imageView;};
private:
    void loadImage(const std::string& path);
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer);
    void createImage();
    void createImageView();
    void createSampler();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
private:
    ::Engine::Renderer::VulkanApi* m_api;

    VkImage m_image;
    VkDeviceMemory m_imageMemory;
    VkImageView m_imageView;
    VkSampler m_imageSampler;
    VkFormat m_format;
    VkImageAspectFlags m_aspectFlags;
    int m_width;
    int m_height;
    VkImageTiling m_tiling;
    VkImageUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;
};

}
}

