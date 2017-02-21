#pragma once
#include "Material.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "ConstantBufferVK.h"
#include "GPUMemoryBlock.h"

class VulkanRenderer;

#define DBOUTW( s )\
{\
std::wostringstream os_;\
os_ << s;\
OutputDebugStringW( os_.str().c_str() );\
}

#define DBOUT( s )\
{\
std::ostringstream os_;\
os_ << s;\
OutputDebugString( os_.str().c_str() );\
}

// use X = {Program or Shader}
#define INFO_OUT(S,X) { \
char buff[1024];\
memset(buff, 0, 1024);\
glGet##X##InfoLog(S, 1024, nullptr, buff);\
DBOUTW(buff);\
}

// use X = {Program or Shader}
#define COMPILE_LOG(S,X,OUT) { \
char buff[1024];\
memset(buff, 0, 1024);\
glGet##X##InfoLog(S, 1024, nullptr, buff);\
OUT=std::string(buff);\
}


class MaterialVK : public Material
{
    friend VulkanRenderer;

    public:
        MaterialVK(const VkDevice& device, const VkPhysicalDevice& physical_device, VkRenderPass* render_pass);
        ~MaterialVK();


        void setShader(const std::string& shaderFileName, ShaderType type);
        void removeShader(ShaderType type);
        int compileMaterial(std::string& errString);
        int enable();
        void disable();
        //GLuint getProgram() { return program; };
        void setDiffuse(Color c);

        // location identifies the constant buffer in a unique way
        void updateConstantBuffer(const void* data, size_t size, unsigned int location);
        // slower version using a string
        void addConstantBuffer(std::string name, unsigned int location);

        void Reset();

        static VkDescriptorSetLayout& GetDescriptorSetLayout();
        static VkDescriptorSet& GetDescriptorSet();
        static VkDescriptorPool& GetDescriptorPool();

        void Build(VkPolygonMode poly_mode);

    private:
        //// map from ShaderType to GL_VERTEX_SHADER, should be static.
        //GLuint mapShaderEnum[4];

        std::map<unsigned int, ConstantBufferVK*> constantBuffers;

        //std::string shaderNames[4];
        std::map<ShaderType, VkShaderStageFlagBits> m_shader_stage_bits_map;

        //// opengl shader object
        //GLuint shaderObjects[4] = { 0,0,0,0 };

        //// TODO: change to PIPELINE
        //// opengl program object
        //GLuint program;
        int compileShader(ShaderType type, std::string& errString);
        std::vector<std::string> expandShaderText(std::string& shaderText, ShaderType type);

        const VkDevice* m_p_device = nullptr;
        const VkPhysicalDevice* m_p_physical_device = nullptr;

        std::map<std::string, GPUMemoryBlock*> m_constant_memory_map;

        std::map<VkShaderStageFlagBits, VkShaderModule> m_shader_module_map;
        std::vector<VkPipelineShaderStageCreateInfo> m_shader_stage_list;

        VkRenderPass* m_p_render_pass;
        static VkDescriptorSetLayout m_desc_set_layout;
        VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
        VkPipeline m_pipeline = VK_NULL_HANDLE;

        static VkDescriptorSet m_descriptor_set;
        static VkDescriptorPool m_descriptor_pool;

        bool m_built = false;
};

