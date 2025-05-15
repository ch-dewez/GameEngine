#include "DescriptorsManager.h"
#include "Core/Renderer/VulkanApi.h"
#include "vulkan/vulkan_core.h"
#include <algorithm>

namespace Engine {
namespace Ressources {


void DescriptorBuilder::Init(){
    DescriptorLayoutCache::Init();
    DescriptorAllocator::Init();
};

void DescriptorBuilder::DestroyAll(){
    DescriptorLayoutCache::Instance().cleanup();
    DescriptorAllocator::Instance().destroy_pools();
};

DescriptorBuilder& DescriptorBuilder::bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
    VkDescriptorSetLayoutBinding newBinding{};

    newBinding.descriptorCount = 1;
    newBinding.descriptorType = type;
    newBinding.pImmutableSamplers = nullptr;
    newBinding.stageFlags = stageFlags;
    newBinding.binding = binding;

    bindings.push_back(newBinding);

    //create the descriptor write
    VkWriteDescriptorSet newWrite{};
    newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    newWrite.pNext = nullptr;

    newWrite.descriptorCount = 1;
    newWrite.descriptorType = type;
    newWrite.pBufferInfo = bufferInfo;
    newWrite.dstBinding = binding;

    writes.push_back(newWrite);
    return *this;
}

DescriptorBuilder& DescriptorBuilder::bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags) {
    VkDescriptorSetLayoutBinding newBinding{};

    newBinding.descriptorCount = 1;
    newBinding.descriptorType = type;
    newBinding.pImmutableSamplers = nullptr;
    newBinding.stageFlags = stageFlags;
    newBinding.binding = binding;

    bindings.push_back(newBinding);

    //create the descriptor write
    VkWriteDescriptorSet newWrite{};
    newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    newWrite.pNext = nullptr;

    newWrite.descriptorCount = 1;
    newWrite.descriptorType = type;
    newWrite.pImageInfo = imageInfo;
    newWrite.dstBinding = binding;

    writes.push_back(newWrite);
    return *this;
}

VkDescriptorSet DescriptorBuilder::build(){
    return build(nullptr);
}

VkDescriptorSet DescriptorBuilder::build(VkDescriptorSetLayout* layoutPointer){
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    
    //build layout first
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = nullptr;

    layoutInfo.pBindings = bindings.data();
    layoutInfo.bindingCount = bindings.size();

    VkDescriptorSet set;
    if (layoutPointer) {
        *layoutPointer = DescriptorLayoutCache::Instance().create_descriptor_layout(&layoutInfo);
        //allocate descriptor
        set = DescriptorAllocator::Instance().allocate(*layoutPointer);
    }else {
        VkDescriptorSetLayout layout = DescriptorLayoutCache::Instance().create_descriptor_layout(&layoutInfo);
        set = DescriptorAllocator::Instance().allocate(layout);
    }

    //write descriptor
    for (VkWriteDescriptorSet& w : writes) {
        w.dstSet = set;
    }

    api.updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);

    return set;
}

DescriptorLayoutCache* layoutCacheInstance = nullptr;

void DescriptorLayoutCache::Init(){
    layoutCacheInstance = new DescriptorLayoutCache();
};
DescriptorLayoutCache& DescriptorLayoutCache::Instance(){
    return *layoutCacheInstance;
};
void DescriptorLayoutCache::ShutDown(){
    delete layoutCacheInstance;
    layoutCacheInstance = nullptr;
};

DescriptorLayoutCache::DescriptorLayoutInfo& DescriptorLayoutCache::DescriptorLayoutInfo::add_binding(uint32_t binding, VkDescriptorType type) {
    VkDescriptorSetLayoutBinding newbind {};
    newbind.binding = binding;
    newbind.descriptorCount = 1;
    newbind.descriptorType = type;

    bindings.push_back(newbind);

    return *this;
}

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const{
    if (other.bindings.size() != bindings.size()){
        return false;
    }
    else {
        //compare each of the bindings is the same. Bindings are sorted so they will match
        for (int i = 0; i < bindings.size(); i++) {
            if (other.bindings[i].binding != bindings[i].binding){
                return false;
            }
            if (other.bindings[i].descriptorType != bindings[i].descriptorType){
                return false;
            }
            if (other.bindings[i].descriptorCount != bindings[i].descriptorCount){
                return false;
            }
            if (other.bindings[i].stageFlags != bindings[i].stageFlags){
                return false;
            }
        }
        return true;
    }
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const{
    using std::size_t;
    using std::hash;

    size_t result = hash<size_t>()(bindings.size());

    for (const VkDescriptorSetLayoutBinding& b : bindings)
    {
        //pack the binding data into a single int64. Not fully correct but it's ok
        size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

        //shuffle the packed binding data and xor it with the main hash
        result ^= hash<size_t>()(binding_hash);
    }

    return result;
}

void DescriptorLayoutCache::cleanup(){
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    //delete every descriptor layout held
    for (auto pair : m_layoutCache){
        api.destroyDescriptorSetLayout(pair.second, nullptr);
    }
}

VkDescriptorSetLayout DescriptorLayoutCache::create_descriptor_layout(VkDescriptorSetLayoutCreateInfo* info){
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();

    DescriptorLayoutInfo layoutinfo;
    layoutinfo.bindings.reserve(info->bindingCount);
    bool isSorted = true;
    int lastBinding = -1;

    //copy from the direct info struct into our own one
    for (int i = 0; i < info->bindingCount; i++) {
        layoutinfo.bindings.push_back(info->pBindings[i]);

        //check that the bindings are in strict increasing order
        if (info->pBindings[i].binding > lastBinding){
            lastBinding = info->pBindings[i].binding;
        }
        else{
            isSorted = false;
        }
    }
    //sort the bindings if they aren't in order
    if (!isSorted){
        std::sort(layoutinfo.bindings.begin(), layoutinfo.bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b ){
            return a.binding < b.binding;
        });
    }

    //try to grab from cache
    auto it = m_layoutCache.find(layoutinfo);
    if (it != m_layoutCache.end()){
        return (*it).second;
    }
    else {
        //create a new one (not found)
        VkDescriptorSetLayout layout;
        api.createDescriptorSetLayout(info, nullptr, &layout);

        //add to cache
        m_layoutCache[layoutinfo] = layout;
        return layout;
    }
}

DescriptorAllocator* allocatorInstance = nullptr;

void DescriptorAllocator::Init(){
    allocatorInstance = new DescriptorAllocator();
};
DescriptorAllocator& DescriptorAllocator::Instance(){
    return *allocatorInstance;
};
void DescriptorAllocator::ShutDown(){
    delete allocatorInstance;
    allocatorInstance = nullptr;
};

VkDescriptorPool DescriptorAllocator::get_pool()
{       
    VkDescriptorPool newPool;
    if (readyPools.size() != 0) {
        newPool = readyPools.back();
        readyPools.pop_back();
    }
    else {
        //need to create a new pool
        newPool = create_pool(setsPerPool, ratios);

        setsPerPool = setsPerPool * 1.5;
        if (setsPerPool > 4092) {
            setsPerPool = 4092;
        }
    }   

    return newPool;
}

VkDescriptorPool DescriptorAllocator::create_pool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (PoolSizeRatio ratio : poolRatios) {
        poolSizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * setCount)
        });
    }

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = 0;
    pool_info.maxSets = setCount;
    pool_info.poolSizeCount = (uint32_t)poolSizes.size();
    pool_info.pPoolSizes = poolSizes.data();

    VkDescriptorPool newPool;

    Renderer::VulkanApi::Instance().createDescriptorPool(&pool_info, nullptr, &newPool);
    return newPool;
}

DescriptorAllocator::DescriptorAllocator(uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
{
    ratios.clear();

    for (auto r : poolRatios) {
        ratios.push_back(r);
    }

    VkDescriptorPool newPool = create_pool(maxSets, poolRatios);

    setsPerPool = maxSets * 1.5; //grow it next allocation

    readyPools.push_back(newPool);
}

DescriptorAllocator::DescriptorAllocator() {
    std::vector<DescriptorAllocator::PoolSizeRatio> poolRatios = { 
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 3 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
    };

    uint32_t maxSets = 1000;

    ratios.clear();

    for (auto r : poolRatios) {
        ratios.push_back(r);
    }

    VkDescriptorPool newPool = create_pool(maxSets, poolRatios);

    setsPerPool = maxSets * 1.5; //grow it next allocation

    readyPools.push_back(newPool);
}

void DescriptorAllocator::clear_pools()
{ 
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    for (auto p : readyPools) {
        api.resetDescriptorPool(p, 0);
    }
    for (auto p : fullPools) {
        api.resetDescriptorPool(p, 0);
        readyPools.push_back(p);
    }
    fullPools.clear();
}

void DescriptorAllocator::destroy_pools()
{
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    for (auto p : readyPools) {
        api.destroyDescriptorPool(p, nullptr);
    }
    readyPools.clear();
    for (auto p : fullPools) {
        api.destroyDescriptorPool(p, nullptr);
    }
    fullPools.clear();
}

VkDescriptorSet DescriptorAllocator::allocate(VkDescriptorSetLayout layout, void* pNext)
{
    //get or create a pool to allocate from
    VkDescriptorPool poolToUse = get_pool();

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.pNext = pNext;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = poolToUse;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();

    VkDescriptorSet ds;
    VkResult result = api.allocateDescriptorSets(&allocInfo, &ds);

    //allocation failed. Try again
    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {

        fullPools.push_back(poolToUse);

        poolToUse = get_pool();
        allocInfo.descriptorPool = poolToUse;

        api.allocateDescriptorSets(&allocInfo, &ds);
    }

    readyPools.push_back(poolToUse);
    return ds;
}

}
}
