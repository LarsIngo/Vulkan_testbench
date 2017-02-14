#pragma once
#include <vulkan/vulkan.h>
#include "ConstantBuffer.h"
#include "GPUMemoryBlock.h"

class ConstantBufferVK : public ConstantBuffer
{
    public:
        ConstantBufferVK(const VkDevice& device, const VkPhysicalDevice& physical_device);
        ~ConstantBufferVK();
        void setData(const void* data, size_t size, Material* m, unsigned int location);
        void bind(Material*);

        void Reset();

    private:

        //std::string name;
        //uint32_t location;
        //GLuint location;
        //GLuint handle;
        //GLuint index;
        //void* buff = nullptr;
        //void* lastMat;

        const VkDevice* m_p_device = nullptr;
        const VkPhysicalDevice* m_p_physical_device = nullptr;

        GPUMemoryBlock* m_gpu_memory = nullptr;
};

