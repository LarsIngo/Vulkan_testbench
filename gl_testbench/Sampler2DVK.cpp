//#include <GL/glew.h>
#include "Sampler2DVK.h"
#include "vkTools.hpp"

// enum WRAPPING { REPEAT = 0, CLAMP = 1 };
// enum FILTER { POINT = 0, LINEAR = 0 };
//GLuint wrapMap[2] = { GL_REPEAT, GL_CLAMP };
//GLuint filterMap[2] = { GL_NEAREST, GL_LINEAR };


Sampler2DVK::Sampler2DVK(const VkDevice& device, const VkPhysicalDevice& physical_device)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;
    //glGenSamplers(1, &samplerHandler);
    //// defaults
    //minFilter = magFilter = GL_NEAREST;
    //wrapS = wrapT = GL_CLAMP;

    VkSamplerCreateInfo sampler_create_info = {};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.anisotropyEnable = VK_TRUE;
    sampler_create_info.maxAnisotropy = 16;
    sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_create_info.unnormalizedCoordinates = VK_FALSE;
    sampler_create_info.compareEnable = VK_FALSE;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.minLod = 0.0f;
    sampler_create_info.maxLod = 0.0f;

    vkTools::VkErrorCheck(vkCreateSampler(*m_p_device, &sampler_create_info, nullptr, &m_sampler));
}

Sampler2DVK::~Sampler2DVK()
{
    vkDestroySampler(*m_p_device, m_sampler, nullptr);
    //glDeleteSamplers(1, &samplerHandler);
}

void Sampler2DVK::setMagFilter(FILTER filter)
{
    //magFilter = filterMap[filter];
}


void Sampler2DVK::setMinFilter(FILTER filter)
{
    //minFilter = filterMap[filter];
}

void Sampler2DVK::setWrap(WRAPPING s, WRAPPING t)
{
    //wrapS = wrapMap[s];
    //wrapT = wrapMap[t];
}

VkSampler& Sampler2DVK::GetSampler()
{
    return m_sampler;
}
