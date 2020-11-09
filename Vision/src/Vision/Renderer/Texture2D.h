#pragma once

#include "Vision/Core/Base.h"

#include <glm/glm.hpp>

namespace Vision
{
    enum class WrapMode
    {
        Repeat,
        ClampToEdge
    };

    enum class FilterMode
    {
        Point,
        Bilinear
    };

    struct TextureData
    {
        std::string Name;
        std::string Path;

        uint32 RendererID = 0;
        uint32 Width      = 0;
        uint32 Height     = 0;
    };

    struct TextureProps
    {
        WrapMode WrapX = WrapMode::Repeat;
        WrapMode WrapY = WrapMode::Repeat;

        FilterMode FilterMode = FilterMode::Point;
    };

    class Texture2D
    {
    public:
        virtual ~Texture2D() {}

        virtual void SetData(void* data, uint32 sizeInBytes) = 0;

        virtual void Bind(uint32 slot) const = 0;

        virtual void SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY) = 0;
        virtual void SetFilterMode(FilterMode filterMode) = 0;

        static Texture2D* Create(uint32 width, uint32 height, const TextureProps& props = TextureProps());
        static Texture2D* CreateFromFile(const std::string& path, const TextureProps& props = TextureProps());

        inline const TextureData& GetData() const { return m_Data; }
        inline const TextureProps& GetProperties() const { return m_Properties; }

    protected:
        TextureData  m_Data;
        TextureProps m_Properties;
    };
}