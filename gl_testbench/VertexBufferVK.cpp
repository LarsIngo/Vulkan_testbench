#include "VertexBufferVK.h"
#include "MeshVK.h"
#include <assert.h>
#include "vkTools.hpp"

//GLuint VertexBufferGL::usageMapping[3] = { GL_STATIC_COPY, GL_DYNAMIC_COPY, GL_DONT_CARE };

std::map<std::size_t, GPUMemoryBlock*> VertexBufferVK::m_s_gpu_memory_map = {};

std::size_t VertexBufferVK::m_s_total_size = 48 * 2001;

std::size_t VertexBufferVK::m_s_offset = 0;

VertexBufferVK::VertexBufferVK(const VkDevice& device, const VkPhysicalDevice& physical_device)// : _handle(0)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;
};

VertexBufferVK::~VertexBufferVK()
{
    //glDeleteBuffers(1, &_handle);
    
    assert(m_s_gpu_memory_map.size() == 0);

    //m_s_gpu_memory_map.clear();
}

void VertexBufferVK::setData(const void* inData, size_t size, DATA_USAGE usage)
{
    m_last_data = inData;

    // delete if memory exists
    //if (_handle > 0) {
    //    glDeleteBuffers(1, &_handle);
    //}

    //GLuint newSSBO;
    //glGenBuffers(1, &newSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usageMapping[usage]);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //_handle = newSSBO;


}

/*
bind at "location", with offset "offset", "size" bytes
*/
void VertexBufferVK::bind(size_t offset, size_t size, unsigned int location) {
    //assert(offset + size <= totalSize);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
    //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, location, _handle, offset, size);

    if (m_s_gpu_memory_map.find(location) == m_s_gpu_memory_map.end())
        m_s_gpu_memory_map[location] = new GPUMemoryBlock(*m_p_device, *m_p_physical_device, m_s_total_size,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_s_offset = m_s_gpu_memory_map[location]->Allocate(size);
    m_s_gpu_memory_map[location]->Update(m_last_data, size, m_s_offset);
}

void VertexBufferVK::unbind() {
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

std::size_t VertexBufferVK::getSize() {
    return m_s_total_size;
}

std::size_t VertexBufferVK::getOffset() {
    return m_s_offset;
}

void VertexBufferVK::Reset()
{
    for (auto& it : m_s_gpu_memory_map)
        it.second->Reset();
}

void VertexBufferVK::Clear()
{
    for (auto& it : m_s_gpu_memory_map)
        delete it.second;
    m_s_gpu_memory_map.clear();
}

VkBuffer* VertexBufferVK::getBuffer(unsigned int location)
{
    assert(m_s_gpu_memory_map.find(location) != m_s_gpu_memory_map.end());
    return &m_s_gpu_memory_map[location]->m_buffer;
}
