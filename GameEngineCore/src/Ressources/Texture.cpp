#include "Texture.h"
#include "Renderer/VulkanApi.h"
#include "vulkan/vulkan_core.h"
#include "Buffer.h"
#include <stb_image.h>
#include <filesystem>

namespace Engine {
namespace Ressources {

Texture::TextureCreateInfo Texture::TextureCreateInfo::getDefault(std::string path) {
    TextureCreateInfo info;
    info.path = std::move(path);
    info.possibleFormats = {VK_FORMAT_R8G8B8_SRGB, VK_FORMAT_R8G8B8A8_SRGB};
    info.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    info.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    info.featureFlags = 0;
    return std::move(info);
}

Texture::TextureCreateInfo Texture::TextureCreateInfo::getDefaultWihtoutPath(int width, int height) {
    TextureCreateInfo info;
    info.possibleFormats = {VK_FORMAT_R8G8B8_SRGB, VK_FORMAT_R8G8B8A8_SRGB};
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    info.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    info.featureFlags = 0;
    return std::move(info);
}

Texture::TextureCreateInfo Texture::TextureCreateInfo::getDefaultForDepthBuffer(int width, int height) {
    TextureCreateInfo info;
    info.width = width;
    info.height = height;
    info.possibleFormats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    info.featureFlags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    return std::move(info);
}

void Texture::initMemberFromInfo(TextureCreateInfo& info) {
    m_format = findSupportedFormat(info.possibleFormats, info.tiling, 0);
    m_width = info.width;
    m_height = info.height;
    m_format = findSupportedFormat(info.possibleFormats, info.tiling, 0);
    m_tiling = info.tiling;
    m_usage = info.usage;
    m_properties = info.memoryProperties;
    m_aspectFlags = info.aspectFlags;
    m_tiling = info.tiling;
    m_usage = info.usage;
    m_properties = info.memoryProperties;
}

Texture::Texture(TextureCreateInfo& info){
    initMemberFromInfo(info);
    
    if (info.path != "") {
        loadImage(info.path);
    }else {
        createImage();
    }
    createImageView();
    createSampler();
};

Texture::Texture(TextureCreateInfo& info, ::Engine::Renderer::VulkanApi* api)
{
    m_api = api;

    initMemberFromInfo(info);
    if (info.path != "") {
        loadImage(info.path);
    }else {
        createImage();
    }
    createImageView();
    createSampler();
};

Texture::~Texture() {
    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();
    api.destroyImage(m_image, nullptr);
    api.freeMemory(m_imageMemory, nullptr);
    api.destroyImageView(m_imageView, nullptr);
    api.destroySampler(m_imageSampler, nullptr);
}


VkDescriptorImageInfo Texture::createDescriptorImageInfo(){
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = m_imageView;
    imageInfo.sampler = m_imageSampler;
    return imageInfo;
}

void Texture::loadImage(const std::string& path) {
    //constant for now

    int texChannels;
    stbi_uc* pixels = stbi_load(path.data(), &m_width, &m_height, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = m_width * m_height * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    } 

    Buffer stagingBuffer;
    stagingBuffer.createBaseBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0);

    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();

    void* data;
    stagingBuffer.mapMemory(imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    stagingBuffer.unmapMemory();

    createImage();

    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer.getBuffer());
    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stbi_image_free(pixels);
};

void Texture::createImage() {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_format;
    imageInfo.tiling = m_tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = m_usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();

    if (api.createImage(&imageInfo, nullptr, &m_image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    api.getImageMemoryRequirements(m_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Buffer::findMemoryType(memRequirements.memoryTypeBits, m_properties, api);

    if (api.allocateMemory(&allocInfo, nullptr, &m_imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    api.bindImageMemory(m_image, m_imageMemory, 0);
};

void Texture::createImageView() {
    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_format;
    viewInfo.subresourceRange.aspectMask = m_aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (api.createImageView(&viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
};

void Texture::createSampler() {
    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    VkPhysicalDeviceProperties properties{};
    api.getPhysicalDeviceProperties(&properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE; // true = coord from 0 to width false = 0 to 1
    
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (api.createSampler(&samplerInfo, nullptr, &m_imageSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void Texture::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout){
    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();
    VkCommandBuffer commandBuffer = api.beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    api.cmdPipelineBarrier(commandBuffer, 
                           sourceStage, destinationStage, 
                           0, 
                           0, nullptr, 
                           0, nullptr, 
                           1, &barrier);
    api.endSingleTimeCommands(commandBuffer);
}

void Texture::copyBufferToImage(VkBuffer buffer) {
    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();
    VkCommandBuffer commandBuffer = api.beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        static_cast<uint32_t>(m_width),
        static_cast<uint32_t>(m_height),
        1
    };

    api.cmdCopyBufferToImage(commandBuffer, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    api.endSingleTimeCommands(commandBuffer);
}

VkFormat Texture::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    auto& api = m_api ? *m_api : Renderer::VulkanApi::Instance();
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        api.getPhysicalDeviceFormatProperties(format, &props);
        //if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features && props.optimalTilingFeatures != 0) {
            return format;
        //} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features && props.optimalTilingFeatures != 0) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

}
}
