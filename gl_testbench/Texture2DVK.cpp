#include "Texture2DVK.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "vkTools.hpp"

Texture2DVK::Texture2DVK(const VkDevice& device, const VkPhysicalDevice& physical_device, const VkQueue& queue, const VkCommandPool& command_pool)
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;
    m_p_queue = &queue;
    m_p_command_pool = &command_pool;
}

Texture2DVK::~Texture2DVK()
{
    vkDestroyImage(*m_p_device, m_image, nullptr);
    vkFreeMemory(*m_p_device, m_image_memory, nullptr);
    vkDestroyImageView(*m_p_device, m_image_view, nullptr);

    delete this->sampler;
}

// return 0 if image was loaded and texture created.
// else return -1
int Texture2DVK::loadFromFile(std::string filename)
{
    VkImage init_image = VK_NULL_HANDLE;
    VkDeviceMemory init_image_memory = VK_NULL_HANDLE;

    int width, height, bbp;
    unsigned char* rgb = stbi_load(filename.c_str(), &width, &height, &bbp, STBI_rgb_alpha);
    if (rgb == nullptr)
        return -1;
    int size = 4 * width * height;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

    {   // INIT IMAGE
        vkTools::CreateImage(
            *m_p_device, *m_p_physical_device, width, height,
            format, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, init_image, init_image_memory
        );

        VkImageSubresource subresource = {};
        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource.mipLevel = 0;
        subresource.arrayLayer = 0;

        VkSubresourceLayout subresource_layout;
        vkGetImageSubresourceLayout(*m_p_device, init_image, &subresource, &subresource_layout);

        void* data;
        vkMapMemory(*m_p_device, init_image_memory, 0, size, 0, &data);

        if (subresource_layout.rowPitch == width * 4)
            memcpy(data, rgb, (size_t)size);
        else
        {
            uint8_t* data_bytes = reinterpret_cast<uint8_t*>(data);
            for (int y = 0; y < height; y++)
            {
                memcpy(&data_bytes[y * subresource_layout.rowPitch], &rgb[y * width * 4], width * 4);
            }
        }
        vkUnmapMemory(*m_p_device, init_image_memory);
    }
    
    {   // FINAL IMAGE
        vkTools::CreateImage(*m_p_device, *m_p_physical_device, width, height,
            format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_image_memory);
    }

    {   // LAYOUT TRANSITIONS
        {   // INIT IMAGE
            VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

            vkTools::TransitionImageLayout(
                command_buffer, init_image, format,
                VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
            );

            vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
        }
        {   // IMAGE
            VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

            vkTools::TransitionImageLayout(
                command_buffer, m_image, format,
                VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            );

            vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
        }
    }

    {   // COPY INIT IMAGE TO IMAGE
        VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

        vkTools::CopyImage(command_buffer, init_image, m_image, width, height);

        vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
    }

    {   // TRANSITION IMAGE TO SHADER READ
        VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

        vkTools::TransitionImageLayout(
            command_buffer, m_image, format,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

        vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
    }

    {   // IMAGE VIEW
        vkTools::CreateImageView(*m_p_device, m_image, format, VK_IMAGE_ASPECT_COLOR_BIT, m_image_view);
    }

    //// not 0
    //if (textureHandle)
    //{
    //    glDeleteTextures(1, &textureHandle);
    //};

    //glGenTextures(1, &textureHandle);
    //glBindTexture(GL_TEXTURE_2D, textureHandle);

    //// sampler
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    //if (bpp == 3)
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    //else if (bpp == 4)
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb);

    //glGenerateMipmap(GL_TEXTURE_2D);

    //// release
    //glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(rgb);

    vkDestroyImage(*m_p_device, init_image, nullptr);
    vkFreeMemory(*m_p_device, init_image_memory, nullptr);

    return 0;
}

void Texture2DVK::bind(unsigned int slot)
{
    //glActiveTexture(GL_TEXTURE0 + slot);
    //glBindTexture(GL_TEXTURE_2D, textureHandle);

    //if (this->sampler != nullptr)
    //{
    //    Sampler2DGL* s = (Sampler2DGL*)this->sampler;
    //    glBindSampler(slot, s->samplerHandler);
    //    glSamplerParameteri(s->samplerHandler, GL_TEXTURE_MAG_FILTER, s->magFilter);
    //    glSamplerParameteri(s->samplerHandler, GL_TEXTURE_MIN_FILTER, s->minFilter);
    //    glSamplerParameteri(s->samplerHandler, GL_TEXTURE_WRAP_S, s->wrapS);
    //    glSamplerParameteri(s->samplerHandler, GL_TEXTURE_WRAP_T, s->wrspT);
    //}
}

VkImageView Texture2DVK::GetImageView()
{
    return m_image_view;
}
