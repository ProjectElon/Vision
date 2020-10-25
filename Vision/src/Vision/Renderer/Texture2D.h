#pragma once

#include "Vision/Core/Base.h"

#include <glm/glm.hpp>

namespace Vision
{
    enum class WrapMode
    {
        Repeat = 0,
        ClampToEdge = 1
    };

    enum class FilterMode
    {
        Point = 0,
        Bilinear = 1
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

        virtual void SetData(void* data, uint32_t sizeInBytes) = 0;

        virtual void Bind(uint32_t slot) = 0;

        virtual void SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY) = 0;
        virtual void SetFilterMode(FilterMode filterMode) = 0;

        static Ref<Texture2D> Create(uint32_t width, uint32_t height, const TextureProps& props = TextureProps());
        static Ref<Texture2D> CreateFromFile(const std::string& path, const TextureProps& props = TextureProps());

        inline const TextureData& GetData() const { return m_Data; }
        inline const TextureProps& GetProperties() const { return m_Properties; }

        static const Ref<Texture2D>& GetDefault(); // Temprary

    protected:
        TextureData  m_Data;
        TextureProps m_Properties;
    };
}