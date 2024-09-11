#pragma once

#include "Device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace NNuts {

    class NNDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(NNDevice& device) : m_Device{ device } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<NNDescriptorSetLayout> build() const;

        private:
            NNDevice& m_Device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
        };

        NNDescriptorSetLayout(
            NNDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~NNDescriptorSetLayout();
        NNDescriptorSetLayout(const NNDescriptorSetLayout&) = delete;
        NNDescriptorSetLayout& operator=(const NNDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_DescriptorSetLayout; }

    private:
        NNDevice& m_Device;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

        friend class NNDescriptorWriter;
    };

    class NNDescriptorPool {
    public:
        class Builder {
        public:
            Builder(NNDevice& device) : m_Device{ device } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<NNDescriptorPool> build() const;

        private:
            NNDevice& m_Device;
            std::vector<VkDescriptorPoolSize> m_PoolSizes{};
            uint32_t m_MaxSets = 1000;
            VkDescriptorPoolCreateFlags m_PoolFlags = 0;
        };

        NNDescriptorPool(
            NNDevice& m_Device,
            uint32_t m_MaxSets,
            VkDescriptorPoolCreateFlags m_PoolFlags,
            const std::vector<VkDescriptorPoolSize>& m_PoolSizes);
        ~NNDescriptorPool();
        NNDescriptorPool(const NNDescriptorPool&) = delete;
        NNDescriptorPool& operator=(const NNDescriptorPool&) = delete;

        bool allocateDescriptorSet(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        NNDevice& m_Device;
        VkDescriptorPool m_DescriptorPool;

        friend class NNDescriptorWriter;
    };

    class NNDescriptorWriter {
    public:
        NNDescriptorWriter(NNDescriptorSetLayout& setLayout, NNDescriptorPool& pool);

        NNDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        NNDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        NNDescriptorSetLayout& m_SetLayout;
        NNDescriptorPool& m_Pool;
        std::vector<VkWriteDescriptorSet> m_Writes;
    };

}