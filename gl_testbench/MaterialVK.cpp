#include <windows.h>
#include <streambuf>
#include <sstream>
#include <istream>
#include <fstream>
#include <vector>
#include <set>
#include <assert.h>

#include "MaterialVK.h"
#include "vkTools.hpp"

#include "IA.h"

typedef unsigned int uint;



//// recursive function to split a string by a delimiter
//// easier to read than all that crap using STL...
//void split(std::string text, std::vector<std::string>* temp, const char delim = ' ')
//{
//    int pos = text.find(delim, 0);
//    if (pos == -1)
//        temp->push_back(text);
//    else {
//        temp->push_back(text.substr(0, pos));
//        split(text.substr(pos + 1, text.length() - pos - 1), temp, delim);
//    }
//}

/*
vtx_shader is the basic shader text coming from the .vs file.
strings will be added to the shader in this order:
// - version of GLSL
// - defines from map
// - existing shader code
*/
std::vector<std::string> MaterialVK::expandShaderText(std::string& shaderSource, ShaderType type)
{
    //std::vector<std::string> input_definitions;
    //std::ifstream includeFile("IA.h");
    //std::string includeFileString((std::istreambuf_iterator<char>(includeFile)), std::istreambuf_iterator<char>());
    //includeFile.close();
    std::vector<std::string> result{ "\n\n #version 450\n\0" };
    for (auto define : shaderDefines[type])
        result.push_back(define);
    result.push_back(shaderSource);
    return result;
};

MaterialVK::MaterialVK(const VkDevice& device, const VkPhysicalDevice& physical_device)
{
    /*isValid = false;
    mapShaderEnum[(uint)ShaderType::VS] = GL_VERTEX_SHADER;
    mapShaderEnum[(uint)ShaderType::PS] = GL_FRAGMENT_SHADER;
    mapShaderEnum[(uint)ShaderType::GS] = GL_GEOMETRY_SHADER;
    mapShaderEnum[(uint)ShaderType::CS] = GL_COMPUTE_SHADER;*/

    m_p_device = &device;
    m_p_physical_device = &physical_device;

    //m_p_gpu_memory_block = gpu_memory_block;

    m_shader_stage_bits_map[Material::ShaderType::VS] = VK_SHADER_STAGE_VERTEX_BIT;
    m_shader_stage_bits_map[Material::ShaderType::PS] = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_shader_stage_bits_map[Material::ShaderType::GS] = VK_SHADER_STAGE_GEOMETRY_BIT;
    m_shader_stage_bits_map[Material::ShaderType::CS] = VK_SHADER_STAGE_COMPUTE_BIT;

    isValid = false;
};

MaterialVK::~MaterialVK()
{
    //// delete attached constant buffers
    //for (auto buffer : constantBuffers)
    //{
    //    if (buffer.second != nullptr)
    //    {
    //        delete(buffer.second);
    //        buffer.second = nullptr;
    //    }
    //}

    //// delete shader objects and program.
    //for (auto shaderObject : shaderObjects) {
    //    glDeleteShader(shaderObject);
    //};
    //glDeleteProgram(program);

    for (auto& it : m_shader_module_map)
        vkDestroyShaderModule(*m_p_device, it.second, nullptr);

    vkDestroyRenderPass(*m_p_device, m_render_pass, nullptr);

    vkDestroyPipelineLayout(*m_p_device, m_pipeline_layout, nullptr);
    //vkDestroyPipeline(*m_p_device, m_pipeline, nullptr);
        
};

void MaterialVK::setDiffuse(Color c)
{

}

void MaterialVK::setShader(const std::string& shaderFileName, ShaderType type)
{
    if (shaderFileNames.find(type) != shaderFileNames.end())
    {
        // removeShader is implemented in a concrete class
        removeShader(type);
    }
    shaderFileNames[type] = shaderFileName;
};

// this constant buffer will be bound every time we bind the material
void MaterialVK::addConstantBuffer(std::string name, unsigned int location)
{
    //assert(m_constant_memory_map.find(name) == m_constant_memory_map.end());
    //m_constant_memory_map[name] = new GPUMemoryBlock(*m_p_device, *m_p_physical_device, 4096, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    constantBuffers[location] = new ConstantBufferVK(*m_p_device, *m_p_physical_device);
}

// location identifies the constant buffer in a unique way
void MaterialVK::updateConstantBuffer(const void* data, size_t size, unsigned int location)
{
    constantBuffers[location]->setData(data, size, this, location);
}

void MaterialVK::removeShader(ShaderType type)
{

    //GLuint shader = shaderObjects[(GLuint)type];

    const VkShaderStageFlagBits& stageBit = m_shader_stage_bits_map[type];

    // check if name exists (if it doesn't there should not be a shader here.
    if (m_shader_module_map.find(stageBit) != m_shader_module_map.end())
    {
        assert(0 && "MaterialVK::removeShader");
        return;
    }
    //else if (shader != 0 && program != 0) {
    //    glDetachShader(program, shader);
    //    glDeleteShader(shader);
};

int MaterialVK::compileShader(ShaderType type, std::string& err)
{
    //// index in the the array "shaderObject[]";
    //GLuint shaderIdx = (GLuint)type;

    // open the file and read it to a string "shaderText"
    std::ifstream shaderFile(shaderFileNames[type]);
    std::string shaderText;
    if (shaderFile.is_open()) {
        shaderText = std::string((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
        shaderFile.close();
    }
    else {
        assert(0 && "MaterialVKcompileShader: Cannot find file");
        return -1;
    }

    // make final vector<string> with shader source + defines + GLSL version
    std::vector<std::string> shaderLines = expandShaderText(shaderText, type);

    // debug
    for (auto ex : shaderLines)
        DBOUTW(ex.c_str());

    //// OpenGL wants an array of GLchar* with null terminated strings 
    //const GLchar** tempShaderLines = new const GLchar*[shaderLines.size()];
    //int i = 0;
    //// need "auto&" to force "text" not to be a temp string, as we are using the c_str()
    //for (auto &text : shaderLines)
    //    tempShaderLines[i++] = text.c_str();

    std::string sName = "shader";
    switch (type)
    {
    case Material::ShaderType::VS:
        sName += ".vert";
        break;
    case Material::ShaderType::PS:
        sName += ".frag";
        break;
    case Material::ShaderType::GS:
        sName += ".geom";
        break;
    case Material::ShaderType::CS:
        sName += ".comp";
        break;
    default:
        assert(0 && "MaterialVK::compileShader");
        break;
    }

    // Create new shader.
    std::ofstream sfile(sName);
    assert(sfile.is_open());
    for (auto ex : shaderLines)
        sfile << ex;
    sfile.close();

    // Compile spv.
    std::ofstream bfile("CompileSPV.bat");
    assert(bfile.is_open());
    bfile << "glslangValidator.exe -V " << sName << " -o " << "shader.spv" << std::endl;
    bfile << "timeout 3";
    bfile.close();

    ShellExecute(NULL, L"open", L"CompileSPV.bat", NULL, NULL, SW_SHOWNORMAL);
    Sleep(250);
    // Load spv shader.
    const VkShaderStageFlagBits& stageBit = m_shader_stage_bits_map[type];
    assert(m_shader_module_map.find(stageBit) == m_shader_module_map.end());

    vkTools::CreateShaderModule(*m_p_device, "shader.spv", m_shader_module_map[stageBit]);

    //std::wstring wsName(sName.begin(), sName.end());
    //DeleteFile(wsName.c_str());
    //DeleteFile(L"shader.spv");
    //DeleteFile(L"CompileSPV.bat");

    //GLuint newShader = glCreateShader(mapShaderEnum[shaderIdx]);
    //glShaderSource(newShader, shaderLines.size(), tempShaderLines, nullptr);
    //// ask GL to compile this
    //glCompileShader(newShader);
    //// print error or anything...
    //INFO_OUT(newShader, Shader);
    //std::string err2;
    //COMPILE_LOG(newShader, Shader, err2);
    //shaderObjects[shaderIdx] = newShader;


    return 0;
}

int MaterialVK::compileMaterial(std::string& errString)
{
    // remove all shaders.
    removeShader(ShaderType::VS);
    removeShader(ShaderType::PS);

    // compile shaders
    std::string err;
    if (compileShader(ShaderType::VS, err) < 0) {
        errString = err;
        assert(0 && err.c_str());
        exit(-1);
        return -1;
    };
    if (compileShader(ShaderType::PS, err) < 0) {
        errString = err;
        assert(0 && err.c_str());
        exit(-1);
        return -1;
    };

    //// try to link the program
    //// link shader program (connect vs and ps)
    //if (program != 0)
    //    glDeleteProgram(program);

    //program = glCreateProgram();
    //glAttachShader(program, shaderObjects[(GLuint)ShaderType::VS]);
    //glAttachShader(program, shaderObjects[(GLuint)ShaderType::PS]);
    //glLinkProgram(program);

    //std::string err2;
    //INFO_OUT(program, Program);
    //COMPILE_LOG(program, Program, err2);

    VkFormat tmpFormat = VK_FORMAT_R8G8B8_UNORM;
    vkTools::CreateRenderPass(*m_p_device, tmpFormat, m_render_pass);

    //CreatePipelineLayout
    {
        VkDescriptorSetLayout desc_set_layout;
        {
            std::vector<VkDescriptorSetLayoutBinding> desc_set_layout_binding_list;
            {
                VkDescriptorSetLayoutBinding desc_set_layout_binding;
                desc_set_layout_binding.descriptorCount = 1;
                desc_set_layout_binding.pImmutableSamplers = nullptr;

                desc_set_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                desc_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                desc_set_layout_binding.binding = POSITION;
                desc_set_layout_binding_list.push_back(desc_set_layout_binding);
                desc_set_layout_binding.binding = NORMAL;
                desc_set_layout_binding_list.push_back(desc_set_layout_binding);
                desc_set_layout_binding.binding = TEXTCOORD;
                desc_set_layout_binding_list.push_back(desc_set_layout_binding);

                desc_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                desc_set_layout_binding.binding = TRANSLATION;
                desc_set_layout_binding_list.push_back(desc_set_layout_binding);

                desc_set_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                desc_set_layout_binding.binding = DIFFUSE_TINT;
                desc_set_layout_binding_list.push_back(desc_set_layout_binding);

                desc_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                desc_set_layout_binding.stageFlags =  VK_SHADER_STAGE_FRAGMENT_BIT;
                desc_set_layout_binding.binding = DIFFUSE_SLOT;
                desc_set_layout_binding_list.push_back(desc_set_layout_binding);

            }

            VkDescriptorSetLayoutCreateInfo desc_set_layout_create_info;
            desc_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            desc_set_layout_create_info.pNext = 0;
            desc_set_layout_create_info.flags = 0;
            desc_set_layout_create_info.bindingCount = desc_set_layout_binding_list.size();
            desc_set_layout_create_info.pBindings = desc_set_layout_binding_list.data();

            vkCreateDescriptorSetLayout(*m_p_device, &desc_set_layout_create_info, nullptr, &desc_set_layout);
        }


        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &desc_set_layout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        vkTools::VkErrorCheck(vkCreatePipelineLayout(*m_p_device, &pipelineLayoutInfo, nullptr, &m_pipeline_layout));
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_list;
    for(auto& it : m_shader_module_map)
        shader_stage_list.push_back(vkTools::CreatePipelineShaderStageCreateInfo(*m_p_device, it.second, it.first, "main"));

    //std::vector<VkVertexInputAttributeDescription> shader_input_desc_list = Vertex::GetAttributeDescriptions();
    //VkVertexInputBindingDescription shader_input_bind = Vertex::GetBindingDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    //vertexInputInfo.pVertexBindingDescriptions = &vertex_input_binding_desc;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    //vertexInputInfo.pVertexAttributeDescriptions = vertex_input_attribute_desc_list.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkExtent2D tmpExtent;
    tmpExtent.width = 640;
    tmpExtent.height = 480;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(tmpExtent.width);
    viewport.height = static_cast<float>(tmpExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = tmpExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shader_stage_list.size());
    pipelineInfo.pStages = shader_stage_list.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipeline_layout;
    pipelineInfo.renderPass = m_render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    vkTools::VkErrorCheck(vkCreateGraphicsPipelines(*m_p_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));


    isValid = true;
    return 0;
};

int MaterialVK::enable() {
    //if (program == 0 || isValid == false)
    //    return -1;
    //glUseProgram(program);

    //for (auto cb : constantBuffers)
    //{
    //    cb.second->bind(this);
    //}
    return 0;
};

void MaterialVK::disable() {
    //glUseProgram(0);
};

//int MaterialGL::updateAttribute(
//	ShaderType type,
//	std::string &attribName,
//	void* data,
//	unsigned int size)
//{
//	return 0;
//}

void MaterialVK::Reset()
{
    for (auto& it : constantBuffers)
        it.second->Reset();
}
