#pragma once

#include "pch.h"
#include "Vision/Renderer/Sprite.h"
#include "Vision/Renderer/Texture2D.h"

namespace Vision
{
    class SpriteAtlas
    {
    public:
        Ref<Texture2D> Texture;

        SpriteAtlas(const std::string& name, const Ref<Texture2D>& texture);
        ~SpriteAtlas();

        // (bottomLeft, topRight) in uv space
        void SetSpriteUV(const std::string& name, const glm::vec2& bottomLeft, const glm::vec2& topRight);
        
        // (topLeft, bottomRight) in texture space
        void SetSprite(const std::string& name, glm::vec2 topLeft, glm::vec2 bottomRight);

        // (x, y, width, height) in texture space
        void SetSprite(const std::string& name, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        Ref<Sprite> GetSprite(const std::string& name);

    private:
        std::string m_Name; // For debug purposes
        std::unordered_map<std::string, Ref<Sprite>> m_Sprites;
    };
}