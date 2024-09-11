#include "Descriptor.h"

#include <cassert>
#include <stdexcept>

namespace NNuts {
    // *************** Descriptor Set Layout Builder *********************

    NNDescriptorSetLayout::Builder& NNDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
        assert(m_Bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        m_Bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<NNDescriptorSetLayout> NNDescriptorSetLayout::Builder::build() const {
        return std::make_unique<NNDescriptorSetLayout>(m_Device, m_Bindings);
    }

    // *************** Descriptor Set Layout *********************

    NNDescriptorSetLayout::NNDescriptorSetLayout(
        NNDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : m_Device{ device }, m_Bindings{ bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto& kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo m_DescriptorSetLayoutInfo{};
        m_DescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        m_DescriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        m_DescriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            m_Device.device(),
            &m_DescriptorSetLayoutInfo,
            nullptr,
            &m_DescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    NNDescriptorSetLayout::~NNDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_Device.device(), m_DescriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    NNDescriptorPool::Builder& NNDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
        m_PoolSizes.push_back({ descriptorType, count });
        return *this;
    }

    NNDescriptorPool::Builder& NNDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
        m_PoolFlags = flags;
        return *this;
    }
    NNDescriptorPool::Builder& NNDescriptorPool::Builder::setMaxSets(uint32_t count) {
        m_MaxSets = count;
        return *this;
    }

    std::unique_ptr<NNDescriptorPool> NNDescriptorPool::Builder::build() const {
        return std::make_unique<NNDescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
    }

    // *************** Descriptor Pool *********************

    NNDescriptorPool::NNDescriptorPool(
        NNDevice& device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : m_Device{ device } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(m_Device.device(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    NNDescriptorPool::~NNDescriptorPool() {
        vkDestroyDescriptorPool(m_Device.device(), m_DescriptorPool, nullptr);
    }

    bool NNDescriptorPool::allocateDescriptorSet(
        const VkDescriptorSetLayout m_DescriptorSetLayout, VkDescriptorSet& descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.pSetLayouts = &m_DescriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_Device.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void NNDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            m_Device.device(),
            m_DescriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void NNDescriptorPool::resetPool() {
        vkResetDescriptorPool(m_Device.device(), m_DescriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    NNDescriptorWriter::NNDescriptorWriter(NNDescriptorSetLayout& setLayout, NNDescriptorPool& pool)
        : m_SetLayout{ setLayout }, m_Pool{ pool } {}

    NNDescriptorWriter& NNDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    NNDescriptorWriter& NNDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo) {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    bool NNDescriptorWriter::build(VkDescriptorSet& set) {
        bool success = m_Pool.allocateDescriptorSet(m_SetLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void NNDescriptorWriter::overwrite(VkDescriptorSet& set) {
        for (auto& write : m_Writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_Pool.m_Device.device(), m_Writes.size(), m_Writes.data(), 0, nullptr);
    }
}