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
}

void VertexBufferVK::setData(const void* inData, size_t size, DATA_USAGE usage)
{
    m_size = size;
    // delete if memory exists
    //if (_handle > 0) {
    //    glDeleteBuffers(1, &_handle);
    //}

    m_offset = m_p_gpu_memory->Allocate(inData, size);
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
}

void VertexBufferVK::unbind() {
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

inline size_t VertexBufferVK::getSize() {
    return m_size;
}

inline size_t VertexBufferVK::getOffset() {
    return m_offset;
}
