#include "VertexBufferVK.h"
#include "MeshVK.h"
#include <assert.h>
#include "vkTools.hpp"

//GLuint VertexBufferGL::usageMapping[3] = { GL_STATIC_COPY, GL_DYNAMIC_COPY, GL_DONT_CARE };

VertexBufferVK::VertexBufferVK(const VkDevice& device, const VkPhysicalDevice& physical_device, GPUMemoryBlock* gpu_memory)// : _handle(0)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;
    m_p_gpu_memory = gpu_memory;
};

VertexBufferVK::~VertexBufferVK()
{
    //glDeleteBuffers(1, &_handle);

    vkDestroyBuffer(*m_p_device, m_vertex_buffer, nullptr);
    m_vertex_buffer = VK_NULL_HANDLE;
}

void VertexBufferVK::setData(const void* data, size_t size, DATA_USAGE usage)
{
    totalSize = size;
    // delete if memory exists
    //if (_handle > 0) {
    //    glDeleteBuffers(1, &_handle);
    //}
    assert(m_vertex_buffer == VK_NULL_HANDLE);

    //GLuint newSSBO;
    //glGenBuffers(1, &newSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usageMapping[usage]);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //_handle = newSSBO;
    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;// VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkTools::VkErrorCheck(vkCreateBuffer(*m_p_device, &buffer_create_info, nullptr, &m_vertex_buffer));

    assert(m_p_gpu_memory->m_offset + size <= m_p_gpu_memory->m_total);
    vkTools::VkErrorCheck(vkBindBufferMemory(*m_p_device, m_vertex_buffer, m_p_gpu_memory->m_gpu_memory, m_p_gpu_memory->m_offset));

    void* gpu_data;
    vkMapMemory(*m_p_device, m_p_gpu_memory->m_gpu_memory, m_p_gpu_memory->m_offset, size, 0, &gpu_data);
    memcpy(gpu_data, data, (size_t)buffer_create_info.size );
    vkUnmapMemory(*m_p_device, m_p_gpu_memory->m_gpu_memory);

    m_p_gpu_memory->m_offset += size;
}

/*
bind at "location", with offset "offset", "size" bytes
*/
void VertexBufferVK::bind(size_t offset, size_t size, unsigned int location) {
    //assert(offset + size <= totalSize);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
    //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, location, _handle, offset, size);
}

void VertexBufferVK::unbind() {
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

inline size_t VertexBufferVK::getSize() {
    return totalSize;
}
