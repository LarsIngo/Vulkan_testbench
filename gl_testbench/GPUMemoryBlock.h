#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class GPUMemoryBlock
{
    public:
        GPUMemoryBlock(const VkDevice& device, const VkPhysicalDevice& physical_device, std::size_t memory_size);
        ~GPUMemoryBlock();
        std::size_t m_total;
        std::size_t m_offset;

        VkDeviceMemory m_gpu_memory = VK_NULL_HANDLE;
        const VkDevice* m_p_device;
        const VkPhysicalDevice* m_p_physical_device;
    private:
        
};
