#pragma once

#include "Renderer.h"
#include <vector>
#include <glm/glm.hpp>
#include "ConstantBufferVK.h"
#include "MaterialVK.h"
#include "VertexBufferVK.h"

#define BUILD_ENABLE_VULKAN_DEBUG 1
#include <vulkan/vulkan.h>
#pragma comment(lib,"vulkan-1.lib")
#include "vkTools.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma comment(lib,"glfw3.lib")

class VulkanRenderer : public Renderer
{
public:
    // Constructor.
    VulkanRenderer();

    // Destructor.
    ~VulkanRenderer();

    Material* makeMaterial();
    Mesh* makeMesh();
    VertexBuffer* makeVertexBuffer();
    ConstantBuffer* makeConstantBuffer(std::string NAME, unsigned int location);
    ResourceBinding* makeResourceBinding();
    RenderState* makeRenderState();
    Technique* makeTechnique() {};
    Texture2D* makeTexture2D();
    Sampler2D* makeSampler2D();
    std::string getShaderPath();
    std::string getShaderExtension();

    int initialize(unsigned int width = 640, unsigned int height = 480);
    int shutdown();

    void setClearColor(float, float, float, float);
    void clearBuffer(unsigned int);
    void setRenderTarget(RenderTarget* rt); // complete parameters
    void setRenderState(RenderState* ps);
    void submit(Mesh* mesh);
    void frame();
    void present();

    // Window.
    GLFWwindow* m_window = nullptr;
    unsigned int m_width = 0;
    unsigned int m_height = 0;

    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static VkVertexInputBindingDescription GetBindingDescription() {
            VkVertexInputBindingDescription binding_description = {};
            binding_description.binding = 0;
            binding_description.stride = sizeof(Vertex);
            binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return binding_description;
        }

        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
            std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
            attribute_descriptions[0].binding = 0;
            attribute_descriptions[0].location = 0;
            attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attribute_descriptions[0].offset = offsetof(Vertex, position);
            attribute_descriptions[1].binding = 0;
            attribute_descriptions[1].location = 1;
            attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attribute_descriptions[1].offset = offsetof(Vertex, color);
            return attribute_descriptions;
        }
    };

private:

    // Vulkan.
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_gpu = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties m_physical_device_proterties = {};
    uint32_t m_graphics_family_index = 0;
    uint32_t m_present_family_index = 0;

    std::vector<const char*> m_instance_layer_list;
    std::vector<const char*> m_instance_extension_list;
    std::vector<const char*> m_device_layer_list;
    std::vector<const char*> m_device_extension_list;
    VkDebugReportCallbackEXT m_debug_callback;
    VkDebugReportCallbackCreateInfoEXT m_debug_callback_create_info = {};

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR m_surface_capabilities = {};
    VkSurfaceFormatKHR m_surface_format = {};
    VkExtent2D m_swapchain_extent = {};
    uint32_t m_swapchain_image_count = 0;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapchain_image_list;
    std::vector<VkImageView> m_swapchain_image_view_list;

    std::vector<VertexBufferVK*> m_vertex_buffer_list;

    std::map<unsigned int, ConstantBufferVK*> m_constant_buffer_map;
    std::vector<MaterialVK*> m_material_list;

    VkCommandPool m_command_pool = VK_NULL_HANDLE;
    VkQueue m_graphics_queue = VK_NULL_HANDLE;
    VkQueue m_present_queue = VK_NULL_HANDLE;

    VkSemaphore m_present_complete_semaphore = VK_NULL_HANDLE;
    VkSemaphore m_render_complete_semaphore = VK_NULL_HANDLE;
    uint32_t m_render_swapchain_image_index = 0;
    //std::vector<VkFramebuffer> m_swapchain_framebuffer_list;
    //std::vector<VkCommandBuffer> m_swapchain_command_buffer_list;

    //VkRenderPass m_render_pass = VK_NULL_HANDLE;
    //VkShaderModule m_vert_shader_module = VK_NULL_HANDLE;
    //VkShaderModule m_frag_shader_module = VK_NULL_HANDLE;
    //VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
    //VkPipeline m_graphics_pipeline = VK_NULL_HANDLE;

    // Meshes to draw.
    std::vector<Mesh*> m_draw_list;

    // Wire frame mode.
    bool m_global_wireframe_mode = false;

    // Clear color.
    float m_clear_color[4] = { 0,0,0,0 };

    // Window.
    void m_InitWindow();
    void m_DeInitWindow();

    // Vulkan options.
    void m_SetupLayersAndExtensions();

    // Vulkan instance.
    void m_InitInstance();
    void m_DeInitInstance();

    // Vulkan Debug.
    void m_SetupDebugCallback();
    void m_InitDebug();
    void m_DeInitDebug();

    // Vulkan Device.
    void m_InitDevice();
    void m_DeInitDevice();

    // Window surface.
    void m_InitSurface();
    void m_DeInitSurface();

    // Swapchain.
    void m_InitSwapchain();
    void m_DeInitSwapchain();

    // Swapchain views.
    void m_InitSwapchainImageViews();
    void m_DeInitSwapchainImageViews();

    //// Render pass.
    //void m_InitRenderPass();
    //void m_DeInitRenderPass();

    //// Pipeline.
    //void m_InitGraphicsPipeline();
    //void m_DeInitGraphicsPipeline();

    // Frame buffers.
    //void m_InitFrameBuffers();
    //void m_DeInitFrameBuffers();

    // Allocate device memory.
    void m_InitDeviceMemory();
    void m_DeInitDeviceMemory();

    // Command pool.
    void m_InitCommandPool();
    void m_DeInitCommandPool();

    // Semaphores.
    void m_InitSemaphores();
    void m_DeInitSemaphores();
};
