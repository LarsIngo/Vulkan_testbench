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

    vkDestroyImage(*m_p_device, m_init_image, nullptr);
    vkFreeMemory(*m_p_device, m_init_image_memory, nullptr);

    delete this->sampler;
}

// return 0 if image was loaded and texture created.
// else return -1
int Texture2DVK::loadFromFile(std::string filename)
{
    int width, height, bbp;
    unsigned char* rgb = stbi_load(filename.c_str(), &width, &height, &bbp, STBI_rgb_alpha);
    if (rgb == nullptr)
        return -1;
    int size = 4 * width * height;

    {   // INIT IMAGE
        {
            VkImageCreateInfo image_info = {};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.extent.width = width;
            image_info.extent.height = height;
            image_info.extent.depth = 1;
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
            image_info.tiling = VK_IMAGE_TILING_LINEAR;
            image_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.flags = 0;
            vkTools::VkErrorCheck(vkCreateImage(*m_p_device, &image_info, nullptr, &m_init_image));
        }

        {
            VkMemoryRequirements memory_requirements;
            vkGetImageMemoryRequirements(*m_p_device, m_init_image, &memory_requirements);
            VkMemoryAllocateInfo allocate_info;
            allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocate_info.pNext = NULL;
            allocate_info.allocationSize = size;
            allocate_info.memoryTypeIndex = vkTools::FindMemoryType(*m_p_physical_device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkTools::VkErrorCheck(vkAllocateMemory(*m_p_device, &allocate_info, nullptr, &m_init_image_memory));
            vkBindImageMemory(*m_p_device, m_init_image, m_init_image_memory, 0);
        }

        {
            VkImageSubresource subresource = {};
            subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresource.mipLevel = 0;
            subresource.arrayLayer = 0;

            VkSubresourceLayout subresource_layout;
            vkGetImageSubresourceLayout(*m_p_device, m_init_image, &subresource, &subresource_layout);

            void* data;
            vkMapMemory(*m_p_device, m_init_image_memory, 0, size, 0, &data);

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
            vkUnmapMemory(*m_p_device, m_init_image_memory);
        }
    }
    
    {   // FINAL IMAGE
        {
            VkImageCreateInfo image_info = {};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.extent.width = width;
            image_info.extent.height = height;
            image_info.extent.depth = 1;
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.flags = 0;
            vkTools::VkErrorCheck(vkCreateImage(*m_p_device, &image_info, nullptr, &m_image));
        }

        {
            VkMemoryRequirements memory_requirements;
            vkGetImageMemoryRequirements(*m_p_device, m_image, &memory_requirements);
            VkMemoryAllocateInfo allocate_info;
            allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocate_info.pNext = NULL;
            allocate_info.allocationSize = size;
            allocate_info.memoryTypeIndex = vkTools::FindMemoryType(*m_p_physical_device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkTools::VkErrorCheck(vkAllocateMemory(*m_p_device, &allocate_info, nullptr, &m_image_memory));
            vkBindImageMemory(*m_p_device, m_image, m_image_memory, 0);
        }
    }

    {   // LAYOUT TRANSITIONS
        {   // INIT IMAGE
            VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

            VkImageMemoryBarrier image_memory_barrier;
            image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            image_memory_barrier.pNext = NULL;
            image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.image = m_init_image;
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = 1;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;
            image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(
                command_buffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &image_memory_barrier
            );

            vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
        }
        {   // IMAGE
            VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

            VkImageMemoryBarrier image_memory_barrier;
            image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            image_memory_barrier.pNext = NULL;
            image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.image = m_image;
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = 1;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;
            image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            vkCmdPipelineBarrier(
                command_buffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &image_memory_barrier
            );

            vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
        }
    }

    {   // COPY INIT IMAGE TO IMAGE
        VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

        VkImageSubresourceLayers subresource = {};
        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource.baseArrayLayer = 0;
        subresource.mipLevel = 0;
        subresource.layerCount = 1;

        VkImageCopy region = {};
        region.srcSubresource = subresource;
        region.dstSubresource = subresource;
        region.srcOffset = { 0, 0, 0 };
        region.dstOffset = { 0, 0, 0 };
        region.extent.width = width;
        region.extent.height = height;
        region.extent.depth = 1;

        vkCmdCopyImage(
            command_buffer,
            m_init_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &region
        );

        vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
    }

    {   // TRANSITION IMAGE TO SHADER READ
        VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(*m_p_device, *m_p_command_pool);

        VkImageMemoryBarrier image_memory_barrier;
        image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier.pNext = NULL;
        image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.image = m_image;
        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_memory_barrier.subresourceRange.baseMipLevel = 0;
        image_memory_barrier.subresourceRange.levelCount = 1;
        image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        image_memory_barrier.subresourceRange.layerCount = 1;
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &image_memory_barrier
        );

        vkTools::EndSingleTimeCommand(*m_p_device, *m_p_command_pool, *m_p_queue, command_buffer);
    }

    {   // IMAGE VIEW
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = m_image;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        image_view_create_info.flags = 0;

        vkTools::VkErrorCheck(vkCreateImageView(*m_p_device, &image_view_create_info, nullptr, &m_image_view));
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
