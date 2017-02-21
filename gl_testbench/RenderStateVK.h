#pragma once
#include <vector>
#include "RenderState.h"
#include <vulkan/vulkan.h>

class RenderStateVK : public RenderState
{
    public:
        RenderStateVK();
        ~RenderStateVK();
        void setWireFrame(bool);
        void set();

        VkPolygonMode GetPolygonMode();

        //void setGlobalWireFrame(bool* global);
    private:
        bool m_wireframe;
        //bool* globalWireFrame;
};

