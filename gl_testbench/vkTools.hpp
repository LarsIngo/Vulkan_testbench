#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

namespace vkTools 
{

    void VkErrorCheck( const VkResult& result );

    void AssertErrorMsg( const char* title, const char* msg );

    void OutputErrorMsg( const char* title, const char* msg );

    std::vector<char> ReadFile( const std::string& file_path );

    void CreateShaderModule( const VkDevice& device, const char* shader_spv_path, VkShaderModule& shader_module );

    VkPipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo( const VkDevice& device, const VkShaderModule& shader_module, const VkShaderStageFlagBits& stage_bit, const char* name );

    void CreateRenderPass( const VkDevice& device, const VkFormat& format, VkRenderPass& render_pass );

    void CreateFramebuffer( const VkDevice& device, const VkExtent2D extent, const VkRenderPass& render_pass, const VkImageView& image_view, VkFramebuffer& framebuffer );

    void CreatePipelineLayout( const VkDevice& device, VkPipelineLayout& pipeline_layout );

    void CreateGraphicsPipeline(const VkDevice& device,
        const VkExtent2D& extent,
        const std::vector<VkPipelineShaderStageCreateInfo>& shader_stage_list,
        const std::vector<VkVertexInputAttributeDescription>& vertex_input_attribute_desc_list,
        const VkVertexInputBindingDescription& vertex_input_binding_desc,
        const VkRenderPass& render_pass,
        const VkPipelineLayout& pipeline_layout,
        VkPipeline& graphics_pipeline );

    uint32_t FindGraphicsFamilyIndex( const VkPhysicalDevice& gpu );
    uint32_t FindPresentFamilyIndex( const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface );
    uint32_t FindMemoryType( const VkPhysicalDevice& gpu, const uint32_t& type_filter, const VkMemoryPropertyFlags& memory_property_flags );

    VkCommandBuffer BeginSingleTimeCommand( const VkDevice& device, const VkCommandPool& command_pool );
    void EndSingleTimeCommand( const VkDevice& device, const VkCommandPool& command_pool, const VkQueue& queue, const VkCommandBuffer& command_buffer );

    void CreateCommandBuffer(const VkDevice& device, const VkCommandPool& command_pool, const VkCommandBufferLevel command_buffer_level, VkCommandBuffer& command_buffer);
    void BeginCommandBuffer(const VkCommandBufferUsageFlags command_buffer_useage_flags, const VkCommandBuffer& command_buffer);
    void BeginCommandBuffer(const VkCommandBufferUsageFlags command_buffer_useage_flags, const VkCommandBufferInheritanceInfo command_buffer_inheritance_info, const VkCommandBuffer& command_buffer);
    void EndCommandBuffer( const VkCommandBuffer& command_buffer );
    void SubmitCommandBuffer( const VkQueue& queue, VkCommandBuffer& command_buffer );
    void WaitQueue(const VkQueue& queue );
    void ResetCommandBuffer( VkCommandBuffer& command_buffer );
    void FreeCommandBuffer( const VkDevice& device, const VkCommandPool& command_pool, const VkCommandBuffer& command_buffer );
}
