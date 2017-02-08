#include "GPUMemoryBlock.h"
#include "vkTools.hpp"

GPUMemoryBlock::GPUMemoryBlock(const VkDevice& device, const VkPhysicalDevice& physical_device, std::size_t memory_size)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;

    m_total = memory_size;
    m_offset = 0;

    VkMemoryAllocateInfo memory_allocate_info = {};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_size;
    memory_allocate_info.memoryTypeIndex = 9;

    vkTools::VkErrorCheck(vkAllocateMemory(*m_p_device, &memory_allocate_info, nullptr, &m_gpu_memory));
}

GPUMemoryBlock::~GPUMemoryBlock()
{
    vkFreeMemory(*m_p_device, m_gpu_memory, nullptr);
    m_gpu_memory = VK_NULL_HANDLE;
}
