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

    m_DeInitSemaphores();
    m_DeInitCommandPool();
    m_DeInitDeviceMemory();
    //m_DeInitGraphicsPipeline();
    //m_DeInitRenderPass();
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
    return (Texture2D*)new Texture2DVK();
}

Sampler2D* VulkanRenderer::makeSampler2D()
{
    return (Sampler2D*)new Sampler2DVK();
}

ConstantBuffer* VulkanRenderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
    if (m_constant_buffer_map.find(NAME) == m_constant_buffer_map.end())
    {
        m_constant_buffer_map[NAME] = new ConstantBufferVK(m_device, m_gpu, 16 * 2001);
    }

    return m_constant_buffer_map[NAME];
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
    MaterialVK* mat = new MaterialVK(m_device, m_gpu);
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
    m_InitSwapchainImageViews();
    //m_InitRenderPass();
    //m_InitGraphicsPipeline();
    //m_InitFrameBuffers();
    m_InitDeviceMemory();
    m_InitCommandPool();
    m_InitSemaphores();

    return 0;
}

/*
Super simplified implementation of a work submission
TODO.
*/
void VulkanRenderer::submit(Mesh* mesh)
{
    m_draw_list.push_back(mesh);
}

/*
Naive implementation, no re-ordering, checking for state changes, etc.
TODO.
*/
void VulkanRenderer::frame()
{
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
    VkFramebuffer swapchain_frameBuffer = VK_NULL_HANDLE;

    // START FRAME.
    VkCommandBuffer command_buffer = vkTools::BeginSingleTimeCommand(m_device, m_command_pool);

    for (auto mesh : m_draw_list)
    {
        Technique* t = mesh->technique;
        MaterialVK* m = (MaterialVK*)t->material;

        vkTools::CreateFramebuffer(m_device, m_swapchain_extent, m->m_render_pass, m_swapchain_image_view_list[m_render_swapchain_image_index], swapchain_frameBuffer);

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = m->m_render_pass;
        render_pass_begin_info.framebuffer = swapchain_frameBuffer;
        render_pass_begin_info.renderArea.offset = { 0, 0 };
        render_pass_begin_info.renderArea.extent = m_swapchain_extent;

        VkClearValue clear_color = { m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3] };
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_color;

        vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );
        
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m->m_pipeline );
        
        vkCmdDraw(command_buffer, 3, 1, 0, 0 );
        
        vkCmdEndRenderPass(command_buffer);

        break;
    }


    vkTools::EndSingleTimeCommand(m_device, m_command_pool, m_graphics_queue, command_buffer);
    // END FRAME.

    vkDestroyFramebuffer(m_device, swapchain_frameBuffer, nullptr);

    VkSubmitInfo submit_info = {};
    {
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore signal_semaphores[] = { m_render_complete_semaphore };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;
    }
    vkTools::VkErrorCheck(vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE));


    // RESET DEVICE BUFFERS.
    for (auto& it : m_material_list)
        it->Reset();

    for (auto& it : m_constant_buffer_map)
        it.second->Reset();
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

    *&m_swapchain = new_swapchain;

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
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = m_swapchain_image_list[i];
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
    }
}

void VulkanRenderer::m_DeInitSwapchainImageViews()
{
    for (auto& image_view : m_swapchain_image_view_list) {
        vkDestroyImageView(m_device, image_view, nullptr);
        image_view = VK_NULL_HANDLE;
    }
}

//void VulkanRenderer::m_InitRenderPass()
//{
//    vkTools::CreateRenderPass(m_device, m_surface_format.format, m_render_pass);
//}
//
//void VulkanRenderer::m_DeInitRenderPass()
//{
//    vkDestroyRenderPass(m_device, m_render_pass, nullptr);
//    m_render_pass = VK_NULL_HANDLE;
//}
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

//void VulkanRenderer::m_InitFrameBuffers()
//{
//    m_swapchain_framebuffer_list.resize(m_swapchain_image_count);
//    for ( uint32_t i = 0; i < m_swapchain_image_count; ++i )
//        vkTools::CreateFramebuffer( m_device, m_swapchain_extent, m_render_pass, m_swapchain_image_view_list[ i ], m_swapchain_framebuffer_list[ i ] );
//}
//
//void VulkanRenderer::m_DeInitFrameBuffers()
//{
//    for ( auto& swapchain_frame_buffer : m_swapchain_framebuffer_list ) {
//        vkDestroyFramebuffer( m_device, swapchain_frame_buffer, nullptr );
//        swapchain_frame_buffer = VK_NULL_HANDLE;
//    }
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
}

void VulkanRenderer::m_InitCommandPool()
{
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.queueFamilyIndex = m_graphics_family_index;

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
