#include "BindBufferVK.h"

BindBufferVK::BindBufferVK(const VkDevice& device, const VkPhysicalDevice& physical_device, std::size_t size)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;

    m_gpu_memory = new GPUMemoryBlock(device, physical_device, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_size = size;
}

BindBufferVK::~BindBufferVK()
{
    delete m_gpu_memory;
}

VkBuffer* BindBufferVK::GetBuffer()
{
    return m_gpu_memory->GetBuffer();
}
