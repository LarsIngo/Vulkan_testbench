#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class GPUMemoryBlock
{
    public:
        GPUMemoryBlock(const VkDevice& device, const VkPhysicalDevice& physical_device, std::size_t total_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_property_flags);
        ~GPUMemoryBlock();

        std::size_t Allocate(std::size_t size);

        void Update(const void* inData, std::size_t size, std::size_t offset);

        void Reset();

        std::size_t GetOffset();
        std::size_t GetSize();
        VkBuffer* GetBuffer();

    private:
        std::size_t m_total_size;
        std::size_t m_offset;


        VkDeviceSize m_min_offset_alignment;
        
        VkDeviceMemory m_device_memory = VK_NULL_HANDLE;
        VkBuffer m_buffer = VK_NULL_HANDLE;

        const VkDevice* m_p_device;
        const VkPhysicalDevice* m_p_physical_device;
};
