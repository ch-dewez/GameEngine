//
//
//
//  https://vkguide.dev/docs/new_chapter_4/descriptor_abstractions/
//
//
//

#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <deque>
#include <span>
#include <unordered_map>
#include <vector>

namespace Engine {
namespace Ressources{

// TODO: singleton
class DescriptorLayoutCache {
public:
    static void Init();
    static DescriptorLayoutCache& Instance();
    static void ShutDown();

    void cleanup();

    VkDescriptorSetLayout create_descriptor_layout(VkDescriptorSetLayoutCreateInfo* info);

    struct DescriptorLayoutInfo {
        //good idea to turn this into a inlined array
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        DescriptorLayoutInfo& add_binding(uint32_t binding, VkDescriptorType type);

        bool operator==(const DescriptorLayoutInfo& other) const;

        size_t hash() const;
    };

private:
    struct DescriptorLayoutHash {

        std::size_t operator()(const DescriptorLayoutInfo& k) const{
            return k.hash();
        }
    };

    std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> m_layoutCache;
};


class DescriptorAllocator{
public:

    static void Init();
    static DescriptorAllocator& Instance();
    static void ShutDown();

    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    DescriptorAllocator();
    DescriptorAllocator(uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
    void clear_pools();
    void destroy_pools();

    VkDescriptorSet allocate( VkDescriptorSetLayout layout, void* pNext = nullptr);
private:
    VkDescriptorPool get_pool();
    VkDescriptorPool create_pool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

    std::vector<PoolSizeRatio> ratios;
    std::vector<VkDescriptorPool> fullPools;
    std::vector<VkDescriptorPool> readyPools;
    uint32_t setsPerPool;
};

//TODO: Singleton
class DescriptorBuilder {
public:
    static void Init();
    static void DestroyAll();

    DescriptorBuilder& bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);
    DescriptorBuilder& bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

    VkDescriptorSet build(VkDescriptorSetLayout& layout);
private:

    std::vector<VkWriteDescriptorSet> writes;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};


}
}
