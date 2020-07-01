#include "pch.h"
#include "Vision/Renderer/Sprite.h"
#include <imgui.h>

namespace Vision
{
    Sprite::Sprite(const std::string& name, const Ref<Texture2D>& texture)
        : m_Name(name)
        , Texture(texture)
        , Color(1.0f, 1.0f, 1.0f, 1.0f)
        , FlipX(false)
        , FlipY(false)
        , BottomLeftUV(0.0f, 0.0f)
        , TopRightUV(1.0f, 1.0f)
    {
    }
    
    Sprite::~Sprite()
    {
    }
}