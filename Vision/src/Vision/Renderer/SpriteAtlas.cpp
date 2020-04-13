#include "pch.h"
#include "SpriteAtlas.h"

namespace Vision
{
    SpriteAtlas::SpriteAtlas(const std::string& name, const Ref<Texture2D>& texture)
        : m_Name(name)
        , Texture(texture)
    {
    }

    SpriteAtlas::~SpriteAtlas()
    {
    }

    void SpriteAtlas::SetSpriteUV(const std::string& name, const glm::vec2& bottomLeftUV, const glm::vec2& topRightUV)
    {
        Ref<Sprite> sprite;

        auto it = m_Sprites.find(name);

        if (it == m_Sprites.end())
        {
            auto spriteName = m_Name + "-" + name;
            sprite = CreateRef<Sprite>(spriteName, Texture);
            m_Sprites.emplace(name, sprite);
        }
        else
        {
            sprite = it->second;
        }
        
        sprite->BottomLeftUV = bottomLeftUV;
        sprite->TopRightUV = topRightUV;
    }

    void SpriteAtlas::SetSprite(const std::string& name, glm::vec2 topLeft, glm::vec2 bottomRight)
    {
        // Converting from texture space to UV space
        // TODO : To Be Abstracted into a Rectangle Class

        uint32_t width = Texture->GetWidth();
        uint32_t height = Texture->GetHeight();

        topLeft.y = height - topLeft.y;
        bottomRight.y = height - bottomRight.y;

        glm::vec2 topLeftUV(topLeft.x / width, topLeft.y / height);
        glm::vec2 bottomRightUV(bottomRight.x / width, bottomRight.y / height);

        glm::vec2 bottomLeftUV(topLeftUV.x, bottomRightUV.y);
        glm::vec2 topRightUV(bottomRightUV.x, topLeftUV.y);

        SetSpriteUV(name, bottomLeftUV, topRightUV);
    }

    void SpriteAtlas::SetSprite(const std::string& name, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        SetSprite(name, glm::vec2(x, y), glm::vec2(x + width, y + height));
    }

    Ref<Sprite> SpriteAtlas::GetSprite(const std::string& name)
    {
        auto it = m_Sprites.find(name);
        
        if (it == m_Sprites.end())
        {
            VN_CORE_ERROR("[Sprite Atlas]({0}) Sprite ({1}) Not found", m_Name, name);
            // TODO : Return Pink Texture (Asset System)
        }

        return it->second;
    }
}