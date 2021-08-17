#pragma once

#include "Vision/Core/Defines.h"

namespace Vision
{
    enum ShaderDataType
    {
        ShaderDataType_Bool,
        ShaderDataType_Int8,
        ShaderDataType_UInt8,
        ShaderDataType_Int16,
        ShaderDataType_UInt16,
        ShaderDataType_Int32,
        ShaderDataType_UInt32,
        ShaderDataType_Float,
        ShaderDataType_Float2,
        ShaderDataType_Float3,
        ShaderDataType_Float4,
        ShaderDataType_Matrix3,
        ShaderDataType_Matrix4
    };

    enum ClearFlags : uint32
    {
        ClearFlags_Color   = VnBitU32Mask(0),
        ClearFlags_Depth   = VnBitU32Mask(1),
        ClearFlags_Stencil = VnBitU32Mask(2)
    };

    struct ViewportRect
    {
        uint32 X;
        uint32 Y;
        uint32 Width;
        uint32 Height;
    };

    struct RendererCapabilites
    {
        int32 MaxTextureSlots;
    };
}