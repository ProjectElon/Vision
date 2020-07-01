#include "pch.h"
#include "Vision/Renderer/FrameBuffer.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
    #include "Vision/Renderer/OpenGL/OpenGLFrameBuffer.h"
#endif

namespace Vision
{
    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferProps& frameBufferPros)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::OpenGL:
            {
                return CreateRef<OpenGLFrameBuffer>(frameBufferPros);
            }
            break;
        }
    }
}