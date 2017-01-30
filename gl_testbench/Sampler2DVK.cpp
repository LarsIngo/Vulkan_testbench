//#include <GL/glew.h>
#include "Sampler2DVK.h"

// enum WRAPPING { REPEAT = 0, CLAMP = 1 };
// enum FILTER { POINT = 0, LINEAR = 0 };
//GLuint wrapMap[2] = { GL_REPEAT, GL_CLAMP };
//GLuint filterMap[2] = { GL_NEAREST, GL_LINEAR };


Sampler2DVK::Sampler2DVK()
{
    //glGenSamplers(1, &samplerHandler);
    //// defaults
    //minFilter = magFilter = GL_NEAREST;
    //wrapS = wrapT = GL_CLAMP;
}

Sampler2DVK::~Sampler2DVK()
{
    //glDeleteSamplers(1, &samplerHandler);
}

void Sampler2DVK::setMagFilter(FILTER filter)
{
    //magFilter = filterMap[filter];
}


void Sampler2DVK::setMinFilter(FILTER filter)
{
    //minFilter = filterMap[filter];
}

void Sampler2DVK::setWrap(WRAPPING s, WRAPPING t)
{
    //wrapS = wrapMap[s];
    //wrapT = wrapMap[t];
}

