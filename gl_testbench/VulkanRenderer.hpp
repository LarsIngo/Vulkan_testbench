#pragma once

#define BUILD_ENABLE_VULKAN_DEBUG 1

#include "Renderer.h"
#include <SDL.h>
#include <vector>
#include <vulkan/vulkan.h>

#pragma comment(lib,"vulkan-1.lib")
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")

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

private:
    // Window.
    SDL_Window* m_window = nullptr;
    unsigned int m_width = 0;
    unsigned int m_height = 0;

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

    // Meshes to draw.
    std::vector<Mesh*> m_draw_list;

    // Wire frame mode.
    bool m_global_wireframe_mode = false;

    // Clear color.
    float m_clear_color[4] = { 0,0,0,0 };

    // SDL.
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
};
