#include "GPUMemoryBlock.h"
#include "vkTools.hpp"
#include <assert.h>

GPUMemoryBlock::GPUMemoryBlock(const VkDevice& device, const VkPhysicalDevice& physical_device, std::size_t total_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_property_flags)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;

    m_total_size = total_size;
    m_offset = 0;

    VkPhysicalDeviceProperties physical_device_proterties;
    vkGetPhysicalDeviceProperties(*m_p_physical_device, &physical_device_proterties);
    switch (buffer_usage_flags)
    {
    case VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT:
        m_min_offset_alignment = physical_device_proterties.limits.minUniformBufferOffsetAlignment;
        break;
    case VK_BUFFER_USAGE_STORAGE_BUFFER_BIT:
        m_min_offset_alignment = physical_device_proterties.limits.minStorageBufferOffsetAlignment;
        break;
    default:
        assert(0 && "GPUMemoryBlock::GPUMemoryBlock : NO VALID VkBufferUsageFlags");
        break;
    }

    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = m_total_size;
    buffer_create_info.usage = buffer_usage_flags;// VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;//VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkTools::VkErrorCheck(vkCreateBuffer(*m_p_device, &buffer_create_info, nullptr, &m_buffer));

    {
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(*m_p_device, m_buffer, &memory_requirements);

        VkMemoryAllocateInfo memory_allocate_info = {};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = vkTools::FindMemoryType(*m_p_physical_device, memory_requirements.memoryTypeBits, memory_property_flags);// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        vkTools::VkErrorCheck(vkAllocateMemory(*m_p_device, &memory_allocate_info, nullptr, &m_device_memory));
    }

    vkTools::VkErrorCheck(vkBindBufferMemory(*m_p_device, m_buffer, m_device_memory, 0));
}

GPUMemoryBlock::~GPUMemoryBlock()
{
    vkDestroyBuffer(*m_p_device, m_buffer, nullptr);
    m_buffer = VK_NULL_HANDLE;
    vkFreeMemory(*m_p_device, m_device_memory, nullptr);
    m_device_memory = VK_NULL_HANDLE;
}

size_t GPUMemoryBlock::Allocate(size_t size)
{
    std::size_t offset = m_offset;

    // Alignment
    std::size_t alignment = (m_offset + size) % m_min_offset_alignment;

    assert(offset + size + alignment <= m_total_size);
    m_offset += (size + alignment);
    return offset;
}

void GPUMemoryBlock::Update(const void* inData, std::size_t size, std::size_t offset)
{
    assert(size + offset <= m_total_size);
    void* data;
    vkTools::VkErrorCheck(vkMapMemory(*m_p_device, m_device_memory, offset, size, 0, &data));
    std::memcpy(data, inData, size);
    vkUnmapMemory(*m_p_device, m_device_memory);
}

void GPUMemoryBlock::Reset()
{
    m_offset = 0;
}

std::size_t GPUMemoryBlock::GetOffset()
{
    return m_offset;
}

std::size_t GPUMemoryBlock::GetSize()
{
    return m_total_size;
}

VkBuffer* GPUMemoryBlock::GetBuffer()
{
    return &m_buffer;
}
