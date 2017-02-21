//#include <GL/glew.h>
#include "RenderStateVK.h"

RenderStateVK::RenderStateVK()
{
    m_wireframe = false;
}

RenderStateVK::~RenderStateVK()
{

}

void RenderStateVK::set()
{
    //// have we really changed the wireframe mode?
    //if (*globalWireFrame == _wireframe)
    //    return;
    //else
    //    *globalWireFrame = _wireframe;

    //if (_wireframe)
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //else
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//void RenderStateGL::setGlobalWireFrame(bool* global)
//{
//    this->globalWireFrame = global;
//}

void RenderStateVK::setWireFrame(bool wireframe) {
    m_wireframe = wireframe;
}

VkPolygonMode RenderStateVK::GetPolygonMode()
{
    return m_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
}
