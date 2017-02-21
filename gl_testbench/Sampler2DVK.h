#pragma once
#include "Sampler2D.h"
#include <vulkan/vulkan.h>

class Sampler2DVK : public Sampler2D
{
    public:
        Sampler2DVK(const VkDevice& device, const VkPhysicalDevice& physical_device);
        ~Sampler2DVK();
        void setMagFilter(FILTER filter);
        void setMinFilter(FILTER filter);
        void setWrap(WRAPPING s, WRAPPING t);

        //GLuint magFilter, minFilter, wrapS, wrapT;
        //GLuint samplerHandler = 0;

        VkSampler& GetSampler();

    private:
        VkSampler m_sampler = VK_NULL_HANDLE;

        const VkDevice* m_p_device = nullptr;
        const VkPhysicalDevice* m_p_physical_device = nullptr;
};

