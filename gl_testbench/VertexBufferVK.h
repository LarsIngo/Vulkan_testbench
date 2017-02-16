#pragma once

#include "VertexBuffer.h"
#include <vulkan/vulkan.h>
#include "GPUMemoryBlock.h"
#include <map>

class VertexBufferVK : public VertexBuffer
{
    public:
        VertexBufferVK(const VkDevice& device, const VkPhysicalDevice& physical_device);
        ~VertexBufferVK();

        void setData(const void* data, size_t size, DATA_USAGE usage);
        void bind(size_t offset, size_t size, unsigned int location);
        void unbind();
        size_t getSize();
        size_t getOffset();

        void Reset();
        void Clear();

    private:
        size_t m_size = 0;
        //GLuint _handle;
        size_t m_offset;

        const VkDevice* m_p_device = nullptr;
        const VkPhysicalDevice* m_p_physical_device = nullptr;

        static std::map<std::size_t, GPUMemoryBlock*> m_s_gpu_memory_map;
        const void* m_last_data;
};

