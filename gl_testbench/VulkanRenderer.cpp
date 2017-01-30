#include <stdio.h>
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
    m_InitWindow();
    m_SetupLayersAndExtensions();
    m_SetupDebugCallback();
    m_InitInstance();
    m_InitDebug();
    m_InitDevice();
    m_InitSurface();
    m_InitSwapchain();
    m_InitSwapchainImageViews();
}

VulkanRenderer::~VulkanRenderer()
{
    m_DeInitSwapchainImageViews();
    m_DeInitSwapchain();
    m_DeInitSurface();
    m_DeInitDevice();
    m_DeInitDebug();
    m_DeInitInstance();
    m_DeInitWindow();
}

int VulkanRenderer::shutdown()
{
    //SDL_GL_DeleteContext(context);
    SDL_Quit();
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
    return (ConstantBuffer*)new ConstantBufferVK(NAME, location);
}

std::string VulkanRenderer::getShaderPath()
{
    return std::string("..\\assets\\GL45\\");
}

std::string VulkanRenderer::getShaderExtension()
{
    return std::string(".glsl");
}

VertexBuffer* VulkanRenderer::makeVertexBuffer()
{
    return (VertexBuffer*)new VertexBufferVK();
};

Material* VulkanRenderer::makeMaterial()
{
    return (Material*)new MaterialVK();
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

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "%s", SDL_GetError());
        exit(-1);
    }


    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    m_window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
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
}


void VulkanRenderer::present()
{
    //SDL_GL_SwapWindow(m_window);
}

void VulkanRenderer::setClearColor(float r, float g, float b, float a)
{
    m_clear_color[0] = r; m_clear_color[1] = g; m_clear_color[2] = b; m_clear_color[3] = a;
    //glClearColor(r, g, b, a);
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

}

void VulkanRenderer::m_DeInitWindow()
{

}

void VulkanRenderer::m_SetupLayersAndExtensions()
{

}

void VulkanRenderer::m_SetupDebugCallback()
{

}

void VulkanRenderer::m_InitDebug()
{

}

void VulkanRenderer::m_DeInitDebug()
{

}

void VulkanRenderer::m_InitInstance()
{

}

void VulkanRenderer::m_DeInitInstance()
{

}

void VulkanRenderer::m_InitDevice()
{

}

void VulkanRenderer::m_DeInitDevice()
{

}

void VulkanRenderer::m_InitSurface()
{

}

void VulkanRenderer::m_DeInitSurface()
{

}

void VulkanRenderer::m_InitSwapchain()
{

}

void VulkanRenderer::m_DeInitSwapchain()
{

}

void VulkanRenderer::m_InitSwapchainImageViews()
{

}

void VulkanRenderer::m_DeInitSwapchainImageViews()
{

}
