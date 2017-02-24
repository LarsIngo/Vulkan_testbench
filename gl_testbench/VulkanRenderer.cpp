#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <iostream>
#include <sstream>

#include "VulkanRenderer.h"

#include "MaterialVK.h"
#include "MeshVK.h"
#include "Technique.h"
#include "ResourceBindingVK.h"
#include "RenderStateVK.h"
#include "VertexBufferVK.h"
#include "ConstantBufferVK.h"
#include "Texture2DVK.h"

VulkanRenderer::VulkanRenderer()
{

}

VulkanRenderer::~VulkanRenderer()
{

}

int VulkanRenderer::shutdown()
{
    vkDeviceWaitIdle(m_device);

    m_DeInitDepthBuffer();
    m_DeInitFrameBuffers();
    m_DeInitCommandBuffers();
    m_DeInitSemaphores();
    m_DeInitCommandPool();
    m_DeInitDeviceMemory();
    //m_DeInitGraphicsPipeline();
    m_DeInitRenderPass();
    //m_DeInitFrameBuffers();
    m_DeInitSwapchainImageViews();
    m_DeInitSwapchain();
    m_DeInitSurface();
    m_DeInitDevice();
    m_DeInitDebug();
    m_DeInitInstance();
    m_DeInitWindow();

    //SDL_GL_DeleteContext(context);
    //SDL_Quit();
    return 0;
}

Mesh* VulkanRenderer::makeMesh()
{
    return (Mesh*)new MeshVK();
}

Texture2D* VulkanRenderer::makeTexture2D()
{
    Texture2DVK* tex = new Texture2DVK(m_device, m_gpu, m_graphics_queue, m_command_pool);
    m_texture_list.push_back(tex);
    return (Texture2D*)tex;
}

Sampler2D* VulkanRenderer::makeSampler2D()
{
    Sampler2DVK* samp = new Sampler2DVK(m_device, m_gpu);
    m_sampler_list.push_back(samp);
    return (Sampler2D*)samp;
}

ConstantBuffer* VulkanRenderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
    if (m_constant_buffer_map.find(location) == m_constant_buffer_map.end())
    {
        m_constant_buffer_map[location] = new ConstantBufferVK(m_device, m_gpu, 32 * 2001);
    }

    return m_constant_buffer_map[location];
}

std::string VulkanRenderer::getShaderPath()
{
    return std::string("..\\assets\\Vulkan\\");
}

std::string VulkanRenderer::getShaderExtension()
{
    return std::string(".glsl");
}

VertexBuffer* VulkanRenderer::makeVertexBuffer()
{
    VertexBufferVK* buff = new VertexBufferVK(m_device, m_gpu);
    m_vertex_buffer_list.push_back(buff);
    return (VertexBuffer*)buff;
};

Material* VulkanRenderer::makeMaterial()
{
    MaterialVK* mat = new MaterialVK(m_device, m_gpu, &m_render_pass);
    m_material_list.push_back(mat);
    return (Material*)mat;
}

ResourceBinding* VulkanRenderer::makeResourceBinding()
{
    return (ResourceBinding*)new ResourceBindingVK();
}

RenderState* VulkanRenderer::makeRenderState()
{
    RenderStateVK* newRS = new RenderStateVK();
    //newRS->setGlobalWireFrame(&this->m_global_wireframe_mode);
    //newRS->setWireFrame(false);
    return (RenderState*)newRS;
}

int VulkanRenderer::initialize(unsigned int width, unsigned int height)
{
    //if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    //{
    //    fprintf(stderr, "%s", SDL_GetError());
    //    exit(-1);
    //}


    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

   // m_window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    //context = SDL_GL_CreateContext(window);

    //SDL_GL_MakeCurrent(window, context);

    //glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    //glClearDepth(1.0f);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);

    //glViewport(0, 0, width, height);

    //glewExperimental = GL_TRUE;
    //GLenum err = glewInit();
    //if (GLEW_OK != err)
    //{
    //    fprintf(stderr, "Error GLEW: %s\n", glewGetErrorString(err));
    //}

    m_width = width;
    m_height = height;

    m_InitWindow();
    m_SetupLayersAndExtensions();
    m_SetupDebugCallback();
    m_InitInstance();
    m_InitDebug();
    m_InitDevice();
    m_InitSurface();
    m_InitSwapchain();
    //m_InitGraphicsPipeline();
    //m_InitFrameBuffers();
    m_InitDeviceMemory();
    m_InitCommandPool();
    m_InitSemaphores();
    m_InitCommandBuffers();
    m_InitSwapchainImageViews();
    m_InitDepthBuffer();
    m_InitRenderPass();
    m_InitFrameBuffers();

    return 0;
}

/*
Super simplified implementation of a work submission
TODO.
*/
void VulkanRenderer::submit(Mesh* mesh)
{
    assert(mesh->technique != nullptr);
    MeshEntry entry;
    entry.mesh = mesh;
    entry.index = m_submit_index++;
    m_draw_map[mesh->technique].push_back(entry);
}

/*
Naive implementation, no re-ordering, checking for state changes, etc.
TODO.
*/
void VulkanRenderer::frame()
{
    std::map<MaterialVK*, std::vector<VkWriteDescriptorSet>> write_desc_set_map;
    std::map<MaterialVK*, std::vector<std::uint32_t>> alignment_map;

    std::size_t desc_buff_index = 0;
    std::vector<VkDescriptorImageInfo> all_desc_image_info_list;
    all_desc_image_info_list.resize(15);

    std::size_t desc_image_index = 0;
    std::vector<VkDescriptorBufferInfo> all_desc_buff_info_list;
    all_desc_buff_info_list.resize(15);

    std::vector<VkWriteDescriptorSet> all_write_desc_set_list;

    VkRenderPass& render_pass = m_render_pass;

    for (auto& it : m_draw_map)
    {
        Technique* t = it.first;
        MaterialVK* m = (MaterialVK*)t->material;
        RenderStateVK* r = (RenderStateVK*)t->renderState;
        m->Build(r->GetPolygonMode());

        std::vector<std::uint32_t> alignment_list;
        std::vector<VkWriteDescriptorSet> write_desc_set_list;
        {
            VkWriteDescriptorSet write_desc_set;
            write_desc_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_desc_set.pNext = NULL;
            write_desc_set.dstSet = m->m_descriptor_set;
            write_desc_set.dstArrayElement = 0;
            write_desc_set.descriptorCount = 1;
            write_desc_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            write_desc_set.pImageInfo = NULL;
            { // POSITION
                unsigned int location = POSITION;
                VertexBufferVK* buff = m_vertex_buffer_list[location];
                VkDescriptorBufferInfo& desc_buff_info = all_desc_buff_info_list[desc_buff_index++];
                desc_buff_info = { *buff->GetBuffer(location), 0, buff->GetAlignment(location) };
                write_desc_set.dstBinding = location;
                write_desc_set.pBufferInfo = &desc_buff_info;
                write_desc_set_list.push_back(write_desc_set);
                alignment_list.push_back(buff->GetAlignment(location));
            }
            { // NORMAL
                unsigned int location = NORMAL;
                VertexBufferVK* buff = m_vertex_buffer_list[location];
                VkDescriptorBufferInfo& desc_buff_info = all_desc_buff_info_list[desc_buff_index++];
                desc_buff_info = { *buff->GetBuffer(location), 0, buff->GetAlignment(location) };
                write_desc_set.dstBinding = location;
                write_desc_set.pBufferInfo = &desc_buff_info;
                write_desc_set_list.push_back(write_desc_set);
                alignment_list.push_back(buff->GetAlignment(location));
            }
            { // TEXTCOORD
                unsigned int location = TEXTCOORD;
                VertexBufferVK* buff = m_vertex_buffer_list[location];
                VkDescriptorBufferInfo& desc_buff_info = all_desc_buff_info_list[desc_buff_index++];
                desc_buff_info = { *buff->GetBuffer(location), 0, buff->GetAlignment(location) };
                write_desc_set.dstBinding = location;
                write_desc_set.pBufferInfo = &desc_buff_info;
                write_desc_set_list.push_back(write_desc_set);
                alignment_list.push_back(buff->GetAlignment(location));
            }
            { // TRANSLATION
                unsigned int location = TRANSLATION;
                ConstantBufferVK* buff = m_constant_buffer_map[location];
                VkDescriptorBufferInfo& desc_buff_info = all_desc_buff_info_list[desc_buff_index++];
                desc_buff_info = { *buff->GetBuffer(), 0, buff->GetAlignment() };
                write_desc_set.dstBinding = location;
                write_desc_set.pBufferInfo = &desc_buff_info;
                write_desc_set_list.push_back(write_desc_set);
                alignment_list.push_back(buff->GetAlignment());
            }
            { // DIFFUSE_TINT
                write_desc_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                unsigned int location = DIFFUSE_TINT;
                ConstantBufferVK* cbuff = m->constantBuffers[location];
                VkBuffer buff = *cbuff->GetBuffer();
                VkDescriptorBufferInfo& desc_buff_info = all_desc_buff_info_list[desc_buff_index++];
                desc_buff_info = { buff, 0, cbuff->GetAlignment() };
                write_desc_set.dstBinding = location;
                write_desc_set.pBufferInfo = &desc_buff_info;
                write_desc_set_list.push_back(write_desc_set);
            }
            { // DIFFUSE_SLOT
                write_desc_set.pBufferInfo = NULL;
                unsigned int location = DIFFUSE_SLOT;
                VkDescriptorImageInfo& desc_image_info = all_desc_image_info_list[desc_image_index++];
                desc_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                desc_image_info.imageView = m_texture_list[0]->GetImageView();
                desc_image_info.sampler = m_sampler_list[0]->GetSampler();
                write_desc_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write_desc_set.dstBinding = location;
                write_desc_set.pImageInfo = &desc_image_info;
                write_desc_set_list.push_back(write_desc_set);
            }
        }

        write_desc_set_map[m] = write_desc_set_list;
        alignment_map[m] = alignment_list;

        for(auto& it : write_desc_set_list)
            all_write_desc_set_list.push_back(it);
    }

    //for (auto mesh : m_draw_list)
    //{
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //    for (auto t : mesh->textures)
    //    {
    //        // we do not really know here if the sampler has been
    //        // defined in the shader.
    //        t.second->bind(t.first);
    //    }

    //    Technique* t = mesh->technique;
    //    t->enable(this);

    //    // bind buffers for this mesh.
    //    // this implementation only has buffers in the Vertex Shader!
    //    // bind them all before drawing.
    //    size_t numberElements = 3;
    //    for (auto element : mesh->geometryBuffers) {
    //        mesh->bindIAVertexBuffer(element.first);
    //        numberElements = element.second.numElements;
    //    }

    //    mesh->txBuffer->bind(mesh->technique->material);

    //    // everything is bound!
    //    // always 0 because we are just generating gl_VertexId
    //    glDrawArrays(GL_TRIANGLES, 0, numberElements);
    //}
    //m_draw_list.clear();


    // GET NEXT IMAGE FROM SWAPCHAIN.
    vkTools::VkErrorCheck(vkAcquireNextImageKHR(m_device, m_swapchain, (std::numeric_limits<uint64_t>::max)(), m_present_complete_semaphore, VK_NULL_HANDLE, &m_render_swapchain_image_index));
    assert(m_render_swapchain_image_index <= m_swapchain_image_view_list.size());
    
    //VkFramebuffer swapchain_frameBuffer = VK_NULL_HANDLE;
    //vkTools::CreateFramebuffer(m_device, m_swapchain_extent, m_render_pass, m_swapchain_image_view_list[m_render_swapchain_image_index], swapchain_frameBuffer);
    VkFramebuffer& swapchain_frameBuffer = m_swapchain_framebuffer_list[m_render_swapchain_image_index];

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass;
    render_pass_begin_info.framebuffer = swapchain_frameBuffer;
    render_pass_begin_info.renderArea.offset = { 0, 0 };
    render_pass_begin_info.renderArea.extent = m_swapchain_extent;
    std::vector<VkClearValue> clear_color_list = { { m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3] }, { 1.0f, 0 } };
    render_pass_begin_info.clearValueCount = clear_color_list.size();
    render_pass_begin_info.pClearValues = clear_color_list.data();

    // START FRAME.
    //VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(m_device, m_command_pool);
    VkCommandBuffer& command_buffer = m_swapchain_command_buffer_list[m_render_swapchain_image_index];
    vkTools::BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, command_buffer);

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkUpdateDescriptorSets(m_device, all_write_desc_set_list.size(), all_write_desc_set_list.data(), 0, nullptr);

    for (auto& it : m_draw_map)
    {
        MaterialVK* m = (MaterialVK*)it.first->material;

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m->m_pipeline);

        for (auto& entry : it.second)
        {
            Mesh* mesh = entry.mesh;
            int i = entry.index;

            std::vector<std::uint32_t>& alignment_list = alignment_map[m];
            std::vector<VkWriteDescriptorSet>& write_desc_set_list = write_desc_set_map[m];
            std::vector<std::uint32_t> offset_list;
            offset_list.resize(alignment_list.size());

            for (std::size_t a = 0; a < offset_list.size(); ++a)
                offset_list[a] = alignment_list[a] * i;

            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m->m_pipeline_layout, 0, 1, &m->m_descriptor_set, offset_list.size(), offset_list.data());

            float bias = -(float)i;
            vkCmdSetDepthBias(command_buffer, bias, 0, 0);

            vkCmdDraw(command_buffer, 3, 1, 0, 0);

        }
    }

    vkCmdEndRenderPass(command_buffer);

    //vkTools::EndSingleTimeCommand(m_device, m_command_pool, m_graphics_queue, command_buffer);
    vkTools::EndCommandBuffer(command_buffer);
    vkTools::SubmitCommandBuffer(m_graphics_queue, command_buffer);
    vkTools::WaitQueue(m_graphics_queue);
    vkTools::ResetCommandBuffer(command_buffer);
    // END FRAME.

    //vkDestroyFramebuffer(m_device, swapchain_frameBuffer, nullptr);

    VkSubmitInfo submit_info = {};
    {
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore signal_semaphores[] = { m_render_complete_semaphore };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;
    }
    vkTools::VkErrorCheck(vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE));


    // RESET DEVICE BUFFERS.
    //for (auto& it : m_material_list)
    //    it->Reset();

    for (auto& it : m_constant_buffer_map)
        it.second->Reset();

    m_submit_index = 0;
    m_rendered_frames_count++;
    m_draw_map.clear();
}


void VulkanRenderer::present()
{
    //SDL_GL_SwapWindow(m_window);

    VkPresentInfoKHR present_info = {};
    {
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        VkSemaphore wait_semaphores[] = { m_present_complete_semaphore, m_render_complete_semaphore };
        present_info.waitSemaphoreCount = 2;
        present_info.pWaitSemaphores = wait_semaphores;
        VkSwapchainKHR swapchains[] = { m_swapchain };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &m_render_swapchain_image_index;
    }
    vkQueuePresentKHR(m_present_queue, &present_info);
}

void VulkanRenderer::setClearColor(float r, float g, float b, float a)
{
    m_clear_color[0] = r; m_clear_color[1] = g; m_clear_color[2] = b; m_clear_color[3] = a;
}

void VulkanRenderer::clearBuffer(unsigned int flag)
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //return;
    //// using is only valid inside the function!
    //using namespace CLEAR_BUFFER_FLAGS;
    //GLuint glFlags = BUFFER_MAP[flag & COLOR] | BUFFER_MAP[flag & DEPTH] | BUFFER_MAP[flag & STENCIL];
    //glClear(glFlags);
}

void VulkanRenderer::setRenderTarget(RenderTarget* rt)
{

}

void VulkanRenderer::setRenderState(RenderState* ps)
{
    // naive implementation
    //ps->set();
}

void VulkanRenderer::m_InitWindow()
{
    /* Initialize the library */
    if (!glfwInit()) {
        assert(0 && "GLFWERROR: Initialize the library.");
        std::exit(-1);
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create window */
    m_window = glfwCreateWindow(m_width, m_height, "Vulkan window", NULL, NULL);
}

void VulkanRenderer::m_DeInitWindow()
{
    glfwTerminate();
}

void VulkanRenderer::m_SetupLayersAndExtensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    for (unsigned int i = 0; i < glfw_extension_count; i++)
        m_instance_extension_list.push_back(glfw_extensions[i]);

    m_device_extension_list.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

#if BUILD_ENABLE_VULKAN_DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  obj_type,
    uint64_t                    src_obj,
    size_t                      location,
    int32_t                     msg_code,
    const char*                 layer_prefix,
    const char*                 msg,
    void*                       user_data
)
{
    std::ostringstream stream;
    stream << "VKDBG: ";
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        stream << "INFO";
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        stream << "WARNING";
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        stream << "PERFORMANCE_WARNING";
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        stream << "ERROR";
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        stream << "DEBUG";

    stream << "@[" << layer_prefix << "]: ";
    stream << "Code: " << msg_code << " : " << msg << std::endl;

    std::cout << stream.str();

#ifdef _WIN32
    std::string str = stream.str();
    std::wstring wStr(str.begin(), str.end());
    MessageBox(NULL, wStr.c_str(), L"Vulkan Error!", 0);
#endif
    return VK_FALSE;
}

void VulkanRenderer::m_SetupDebugCallback()
{
    m_debug_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    m_debug_callback_create_info.pfnCallback = (PFN_vkDebugReportCallbackEXT)VulkanDebugCallback;
    m_debug_callback_create_info.flags =
        //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        //VK_DEBUG_REPORT_DEBUG_BIT_EXT |
        0;

    m_instance_layer_list.push_back("VK_LAYER_LUNARG_standard_validation");
    m_instance_extension_list.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    m_device_layer_list.push_back("VK_LAYER_LUNARG_standard_validation");
}


PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = VK_NULL_HANDLE;
PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = VK_NULL_HANDLE;
PFN_vkDebugReportMessageEXT fvkDebugReportMessageEXT = VK_NULL_HANDLE;

void VulkanRenderer::m_InitDebug()
{
    if (m_instance == VK_NULL_HANDLE)
        assert(0 && "VKERROR: NULL instance.");

    fvkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
    fvkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"));
    fvkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(m_instance, "vkDebugReportMessageEXT"));

    if (fvkCreateDebugReportCallbackEXT == VK_NULL_HANDLE || fvkDestroyDebugReportCallbackEXT == VK_NULL_HANDLE) {
        assert(0 && "VKERROR: Could not fetch debug function pointers.");
        std::exit(-1);
    }

    vkTools::VkErrorCheck(fvkCreateDebugReportCallbackEXT(m_instance, &m_debug_callback_create_info, nullptr, &m_debug_callback));
}

void VulkanRenderer::m_DeInitDebug()
{
    fvkDestroyDebugReportCallbackEXT(m_instance, m_debug_callback, nullptr);
    m_debug_callback = VK_NULL_HANDLE;
}

#else

void VulkanRenderer::m_SetupDebugCallback() {};
void VulkanRenderer::m_InitDebug() {};
void VulkanRenderer::m_DeInitDebug() {};

#endif //BUILD_ENABLE_VULKAN_DEBUG

void VulkanRenderer::m_InitInstance()
{
    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.apiVersion = VK_MAKE_VERSION(1, 0, 3);
    application_info.pApplicationName = "Vulkan Renderer";
    application_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    application_info.pEngineName = "No Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(m_instance_extension_list.size());
    instance_create_info.ppEnabledExtensionNames = m_instance_extension_list.data();
    instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(m_instance_layer_list.size());
    instance_create_info.ppEnabledLayerNames = m_instance_layer_list.data();
    instance_create_info.pNext = &m_debug_callback_create_info;

    vkTools::VkErrorCheck(vkCreateInstance(&instance_create_info, nullptr, &m_instance));
}

void VulkanRenderer::m_DeInitInstance()
{
    vkDestroyInstance(m_instance, nullptr);
    m_instance = VK_NULL_HANDLE;
}

void VulkanRenderer::m_InitDevice()
{
    {
        uint32_t gpu_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &gpu_count, nullptr);
        std::vector<VkPhysicalDevice> gpu_list(gpu_count);
        vkEnumeratePhysicalDevices(m_instance, &gpu_count, gpu_list.data());
        m_gpu = gpu_list[0];
        vkGetPhysicalDeviceProperties(m_gpu, &m_physical_device_proterties);
        vkGetPhysicalDeviceFeatures(m_gpu, &m_physical_device_features);
    }

    m_graphics_family_index = vkTools::FindGraphicsFamilyIndex(m_gpu);

    float queue_priorities{ 1.f };
    VkDeviceQueueCreateInfo device_queue_info = {};
    device_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_info.queueCount = 1;
    device_queue_info.queueFamilyIndex = m_graphics_family_index;
    device_queue_info.pQueuePriorities = &queue_priorities;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_info;
    device_create_info.enabledLayerCount = static_cast<uint32_t>(m_device_extension_list.size());
    device_create_info.ppEnabledLayerNames = m_device_extension_list.data();
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extension_list.size());
    device_create_info.ppEnabledExtensionNames = m_device_extension_list.data();
    device_create_info.pEnabledFeatures = &m_physical_device_features;

    vkTools::VkErrorCheck(vkCreateDevice(m_gpu, &device_create_info, nullptr, &m_device));

    vkGetDeviceQueue(m_device, m_graphics_family_index, 0, &m_graphics_queue);
}

void VulkanRenderer::m_DeInitDevice()
{
    vkDestroyDevice(m_device, nullptr);
    m_device = VK_NULL_HANDLE;
}

void VulkanRenderer::m_InitSurface()
{
    vkTools::VkErrorCheck(glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface));

    m_present_family_index = vkTools::FindPresentFamilyIndex(m_gpu, m_surface);

    //vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_gpu, m_surface, &m_surface_capabilities );
    //if ( m_surface_capabilities.currentExtent.width < UINT32_MAX ) {
    //    m_swapchain_extent.width = m_surface_capabilities.currentExtent.width;
    //    m_swapchain_extent.height = m_surface_capabilities.currentExtent.height;
    //}

    {
        uint32_t surface_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &surface_count, nullptr);
        if (surface_count == 0) {
            assert(0 && "VULKANERROR: Surface format missing.");
            std::exit(-1);
        }
        std::vector<VkSurfaceFormatKHR> surface_formats(surface_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &surface_count, surface_formats.data());
        if (surface_formats[0].format == VK_FORMAT_UNDEFINED) {
            m_surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
            m_surface_format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        }
        else {
            m_surface_format = surface_formats[0];
        }
    }
}

void VulkanRenderer::m_DeInitSurface()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    m_surface = VK_NULL_HANDLE;
}

void VulkanRenderer::m_InitSwapchain()
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_gpu, m_surface, &m_surface_capabilities);
    if (m_surface_capabilities.currentExtent.width < UINT32_MAX) {
        m_swapchain_extent.width = m_surface_capabilities.currentExtent.width;
        m_swapchain_extent.height = m_surface_capabilities.currentExtent.height;
    }

    if (m_swapchain_image_count < m_surface_capabilities.minImageCount + 1)
        m_swapchain_image_count = m_surface_capabilities.minImageCount + 1;
    if (m_surface_capabilities.maxImageCount > 0)
        if (m_swapchain_image_count > m_surface_capabilities.maxImageCount)
            m_swapchain_image_count = m_surface_capabilities.maxImageCount;

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR; // VK_PRESENT_MODE_MAILBOX_KHR is guaranteed to work, VK_PRESENT_MODE_MAILBOX_KHR wanted for games (v-sync)
    {
        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &present_mode_count, nullptr);
        std::vector<VkPresentModeKHR> present_mode_list(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &present_mode_count, present_mode_list.data());
        for (auto m : present_mode_list)
            if (m == VK_PRESENT_MODE_MAILBOX_KHR)
                present_mode = m;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = m_surface;
    swapchain_create_info.minImageCount = m_swapchain_image_count;
    swapchain_create_info.imageFormat = m_surface_format.format;
    swapchain_create_info.imageColorSpace = m_surface_format.colorSpace;
    swapchain_create_info.imageExtent.width = m_swapchain_extent.width;
    swapchain_create_info.imageExtent.height = m_swapchain_extent.height;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT when only copy and not render to swapchain image

    uint32_t queue_family_indices[] = { m_graphics_family_index, m_present_family_index };
    if (m_present_family_index != m_graphics_family_index) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // (SLI), several queue families render to same swap chain OR have one queue render and another present 
        swapchain_create_info.queueFamilyIndexCount = 0; // Not used if VK_SHARING_MODE_EXCLUSIVE
        swapchain_create_info.pQueueFamilyIndices = nullptr; // Not used if VK_SHARING_MODE_EXCLUSIVE
    }

    swapchain_create_info.preTransform = m_surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;

    swapchain_create_info.pNext = 0;
    swapchain_create_info.flags = 0;

    VkSwapchainKHR old_swapchain = m_swapchain;
    swapchain_create_info.oldSwapchain = old_swapchain;

    VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
    vkTools::VkErrorCheck(vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr, &new_swapchain));

    m_swapchain = new_swapchain;

    vkTools::VkErrorCheck(vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchain_image_count, nullptr));

    vkGetDeviceQueue(m_device, m_present_family_index, 0, &m_present_queue);
}

void VulkanRenderer::m_DeInitSwapchain()
{
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
}

void VulkanRenderer::m_InitSwapchainImageViews()
{
    m_swapchain_image_list.resize(m_swapchain_image_count);
    m_swapchain_image_view_list.resize(m_swapchain_image_count);

    vkTools::VkErrorCheck(vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchain_image_count, m_swapchain_image_list.data()));

    for (uint32_t i = 0; i < m_swapchain_image_count; ++i) {
        VkImage& swapchain_image = m_swapchain_image_list[i];

        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = swapchain_image;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = m_surface_format.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        vkTools::VkErrorCheck(vkCreateImageView(m_device, &image_view_create_info, nullptr, &m_swapchain_image_view_list[i]));

        VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(m_device, m_command_pool);
        vkTools::TransitionImageLayout(command_buffer, swapchain_image, m_surface_format.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        vkTools::EndSingleTimeCommand(m_device, m_command_pool, m_graphics_queue, command_buffer);
    }
}

void VulkanRenderer::m_DeInitSwapchainImageViews()
{
    for (auto& image_view : m_swapchain_image_view_list) {
        vkDestroyImageView(m_device, image_view, nullptr);
        image_view = VK_NULL_HANDLE;
    }
}

void VulkanRenderer::m_InitRenderPass()
{
    vkTools::CreateRenderPass(m_device, m_surface_format.format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, m_depth_format, m_render_pass);
}

void VulkanRenderer::m_DeInitRenderPass()
{
    vkDestroyRenderPass(m_device, m_render_pass, nullptr);
    m_render_pass = VK_NULL_HANDLE;
}
//
//void VulkanRenderer::m_InitGraphicsPipeline()
//{
//    vkTools::CreateShaderModule( m_device, "shaders/vert.spv", m_vert_shader_module);
//    vkTools::CreateShaderModule( m_device, "shaders/frag.spv", m_frag_shader_module);
//
//    vkTools::CreatePipelineLayout( m_device, m_pipeline_layout );
//    
//    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_list;
//    shader_stage_list.push_back( vkTools::CreatePipelineShaderStageCreateInfo(m_device, m_vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT, "main" ) );
//    shader_stage_list.push_back( vkTools::CreatePipelineShaderStageCreateInfo(m_device, m_frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT, "main" ) );
//
//    std::vector<VkVertexInputAttributeDescription> shader_input_desc_list = Vertex::GetAttributeDescriptions();
//    VkVertexInputBindingDescription shader_input_bind = Vertex::GetBindingDescription();
//    vkTools::CreateGraphicsPipeline( m_device, m_swapchain_extent, shader_stage_list, shader_input_desc_list, shader_input_bind, m_render_pass, m_pipeline_layout, m_graphics_pipeline );
//}
//
//void VulkanRenderer::m_DeInitGraphicsPipeline()
//{
//    vkDestroyPipelineLayout( m_device, m_pipeline_layout, nullptr );
//    vkDestroyPipeline( m_device, m_graphics_pipeline, nullptr );
//    vkDestroyShaderModule( m_device, m_vert_shader_module, nullptr );
//    vkDestroyShaderModule( m_device, m_frag_shader_module, nullptr );
//}

void VulkanRenderer::m_InitDeviceMemory()
{

}

void VulkanRenderer::m_DeInitDeviceMemory()
{
    for (auto& it : m_vertex_buffer_list)
        it->Clear();

    for (auto& it : m_constant_buffer_map)
        delete it.second;

    for (auto& it : m_texture_list)
        delete it;
}

void VulkanRenderer::m_InitCommandPool()
{
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.queueFamilyIndex = m_graphics_family_index;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkTools::VkErrorCheck(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool));
}

void VulkanRenderer::m_DeInitCommandPool()
{
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    m_command_pool = VK_NULL_HANDLE;
}


void VulkanRenderer::m_InitSemaphores()
{
    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkTools::VkErrorCheck(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_render_complete_semaphore));
    vkTools::VkErrorCheck(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_present_complete_semaphore));
}


void VulkanRenderer::m_DeInitSemaphores()
{
    vkDestroySemaphore(m_device, m_render_complete_semaphore, nullptr);
    m_render_complete_semaphore = VK_NULL_HANDLE;
    vkDestroySemaphore(m_device, m_present_complete_semaphore, nullptr);
    m_present_complete_semaphore = VK_NULL_HANDLE;
}

void VulkanRenderer::m_InitCommandBuffers()
{
    m_swapchain_command_buffer_list.resize(m_swapchain_image_count);
    for (uint32_t i = 0; i < m_swapchain_image_count; ++i)
        vkTools::CreateCommandBuffer(m_device, m_command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_swapchain_command_buffer_list[i]);
}

void VulkanRenderer::m_DeInitCommandBuffers()
{
    for (auto& command_buffer : m_swapchain_command_buffer_list) {
        vkTools::FreeCommandBuffer(m_device, m_command_pool, command_buffer);
        command_buffer = VK_NULL_HANDLE;
    }
}

void VulkanRenderer::m_InitFrameBuffers()
{
    m_swapchain_framebuffer_list.resize(m_swapchain_image_count);
    for (uint32_t i = 0; i < m_swapchain_image_count; ++i)
        vkTools::CreateFramebuffer(m_device, m_swapchain_extent, m_render_pass, m_swapchain_image_view_list[i], m_depth_image_view, m_swapchain_framebuffer_list[i]);
}

void VulkanRenderer::m_DeInitFrameBuffers()
{
    for ( auto& swapchain_frame_buffer : m_swapchain_framebuffer_list ) {
        vkDestroyFramebuffer( m_device, swapchain_frame_buffer, nullptr );
        swapchain_frame_buffer = VK_NULL_HANDLE;
    }
}

void VulkanRenderer::m_InitDepthBuffer()
{

    int width = m_swapchain_extent.width;
    int height = m_swapchain_extent.height;
    int size = 1 * width * height;
    m_depth_format = vkTools::FindSupportedFormat(
        m_gpu, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );

    vkTools::CreateImage(m_device, m_gpu, width, height, m_depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_image, m_depth_memory);
    vkTools::CreateImageView(m_device, m_depth_image, m_depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, m_depth_image_view);

    VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(m_device, m_command_pool);
    vkTools::TransitionImageLayout(command_buffer, m_depth_image, m_depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    vkTools::EndSingleTimeCommand(m_device, m_command_pool, m_graphics_queue, command_buffer);
}

void VulkanRenderer::m_DeInitDepthBuffer()
{
    vkDestroyImage(m_device, m_depth_image, nullptr);
    m_depth_image = VK_NULL_HANDLE;
    vkDestroyImageView(m_device, m_depth_image_view, nullptr);
    m_depth_image_view = VK_NULL_HANDLE;
    vkFreeMemory(m_device, m_depth_memory, nullptr);
    m_depth_memory = VK_NULL_HANDLE;
}
