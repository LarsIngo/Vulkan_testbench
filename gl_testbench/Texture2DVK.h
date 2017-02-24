#pragma once
//#include <GL/glew.h>
#include "Texture2D.h"
#include "Sampler2DVK.h"
#include <vulkan/vulkan.h>

class Texture2DVK : public Texture2D
{
    public:
        Texture2DVK(const VkDevice& device, const VkPhysicalDevice& physical_device, const VkQueue& queue, const VkCommandPool& command_pool);
        ~Texture2DVK();

        int loadFromFile(std::string filename);
        void bind(unsigned int slot);

        VkImageView GetImageView();

    private:
        const VkDevice* m_p_device = nullptr;
        const VkPhysicalDevice* m_p_physical_device = nullptr;
        const VkQueue* m_p_queue = nullptr;
        const VkCommandPool* m_p_command_pool = nullptr;

        VkImage m_image = VK_NULL_HANDLE;
        VkDeviceMemory m_image_memory = VK_NULL_HANDLE;
        VkImageView m_image_view = VK_NULL_HANDLE;
};
