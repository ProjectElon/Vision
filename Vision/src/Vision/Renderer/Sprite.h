#pragma once

#include "Vision/Renderer/Texture2D.h"
#include "Vision/Renderer/Shader.h"

namespace Vision
{ 
    class Sprite
    {
    public:
        Ref<Texture2D> Texture;
        glm::vec4      Color;
        glm::vec2      BottomLeftUV;
        glm::vec2      TopRightUV;
        bool           FlipX;
        bool           FlipY;
        
    public:
        Sprite(const std::string& name, const Ref<Texture2D>& texture);
        ~Sprite();
        
        inline const std::string& GetName() const
        { 
            return m_Name;
        }

    private:
        std::string m_Name;
    };
}