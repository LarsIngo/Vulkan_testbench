#pragma once
#include <vulkan/vulkan.h>
#include "GPUMemoryBlock.h"

class BindBufferVK
{
public:
    BindBufferVK(const VkDevice& device, const VkPhysicalDevice& physical_device, std::size_t size);
    ~BindBufferVK();

    VkBuffer* GetBuffer();

private:
    std::size_t m_size;

    const VkDevice* m_p_device = nullptr;
    const VkPhysicalDevice* m_p_physical_device = nullptr;

    GPUMemoryBlock* m_gpu_memory = nullptr;
};

