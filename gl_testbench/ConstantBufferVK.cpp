#include "ConstantBufferVK.h"
#include "MaterialVK.h"
#include <assert.h>

ConstantBufferVK::ConstantBufferVK(const VkDevice& device, const VkPhysicalDevice& physical_device, std::size_t total_size)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;

    m_gpu_memory = new GPUMemoryBlock(device, physical_device, total_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_total_size = total_size;

    //name = NAME;
    //handle = 0;
    //// make a CPU side buffer.
    // binding point (between BUFFER AND SHADER PROGRAM)
    //this->location = location;
    //// location of buffer definition in SHADER PROGRAM
    //index = GL_MAX_UNIFORM_LOCATIONS;
    //lastMat = nullptr;
}

ConstantBufferVK::~ConstantBufferVK()
{
    //if (handle != 0)
    //{
    //    glDeleteBuffers(1, &handle);
    //    handle = 0;
    //};
    delete m_gpu_memory;
}

// this allows us to not know in advance the type of the receiving end, vec3, vec4, etc.
void ConstantBufferVK::setData(const void* data, size_t size, Material* m, unsigned int location)
{

    std::size_t offset = m_gpu_memory->Allocate(size);

    assert(offset + size <= m_total_size);
    
    // delete if memory exists
    //if (_handle > 0) {
    //    glDeleteBuffers(1, &_handle);
    //}

    m_gpu_memory->Update(data, size, offset);

    //assert(this->location == location);
    //if (handle == 0)
    //{
    //    glGenBuffers(1, &handle);
    //    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    //    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    //}
    //else
    //    glBindBuffer(GL_UNIFORM_BUFFER, handle);

    //void* dest = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    //memcpy(dest, data, size);
    //glUnmapBuffer(GL_UNIFORM_BUFFER);

    ////if (buff == nullptr)
    ////{
    ////	buff = new char[size];
    ////}

    //// index of uniform block has not been found yet, or we are using this buffer
    //// with another material!
    //// otherwise we already know the index!, it's fixed at compile time
    //if (index == GL_MAX_UNIFORM_LOCATIONS || lastMat != m)
    //{
    //    lastMat = m;
    //    MaterialGL* mat = (MaterialGL*)m;
    //    index = glGetUniformBlockIndex(mat->getProgram(), name.c_str());
    //    //index = glGetUniformBlockIndex(mat->getProgram(), "TranslationBlock");

    //}
    //// bind CPU side
    ////glBindBuffer(GL_UNIFORM_BUFFER,handle);
    //// set data
    //// set BINDING POINT
    //// stop affecting this buffer.
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ConstantBufferVK::bind(Material* m)
{
    //glBindBuffer(GL_UNIFORM_BUFFER, handle);
    //glBindBufferBase(GL_UNIFORM_BUFFER, location, handle);
}
//
///*
////if (_index = GL_MAX_UNIFORM_LOCATIONS)
////{
////	MaterialGL* material = (MaterialGL*)m;
////	_index = glGetUniformLocation(material->getProgram(), "translate");
////}
////if (_index == 0)
//// _index = glGetUniformBlockIndex(material->getProgram(), "transform");
////glUniformBlockBinding(material->getProgram(), _index, _location);
////glUniform4fv(_location, 1, (float*)_buff);
//}
//*/
//

void ConstantBufferVK::Reset()
{
    m_gpu_memory->Reset();
}

VkBuffer* ConstantBufferVK::GetBuffer()
{
    return m_gpu_memory->GetBuffer();
}

std::size_t ConstantBufferVK::GetTotalSize()
{
    return m_gpu_memory->GetSize();
}

std::size_t ConstantBufferVK::GetOffset()
{
    return m_gpu_memory->GetOffset();
}

VkDeviceSize ConstantBufferVK::GetAlignment()
{
    return m_gpu_memory->GetAlignment();
}
