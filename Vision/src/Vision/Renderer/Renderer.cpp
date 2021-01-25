#include "pch.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"

namespace Vision
{
    uint32 Renderer::GetDataTypeSize(DataType dataType)
    {
        switch (dataType)
        {
            case DataType::Bool:    return 1;         break;
            case DataType::Int8:    return 1;         break;
            case DataType::UInt8:   return 1;         break;
            case DataType::Int16:   return 2;         break;
            case DataType::UInt16:  return 2;         break;
            case DataType::Int32:     return 4;         break;
            case DataType::UInt32:    return 4;         break;
            case DataType::Float:   return 1 * 4;     break;
            case DataType::Float2:  return 2 * 4;     break;
            case DataType::Float3:  return 3 * 4;     break;
            case DataType::Float4:  return 4 * 4;     break;
            case DataType::Matrix3: return 3 * 3 * 4; break;
            case DataType::Matrix4: return 4 * 4 * 4; break;
        }
    }

    uint32 Renderer::GetDataTypeComponentCount(DataType dataType)
    {
        switch (dataType)
        {
            case DataType::Bool:    return 1;     break;
            case DataType::Int8:    return 1;     break;
            case DataType::UInt8:   return 1;     break;
            case DataType::Int16:   return 1;     break;
            case DataType::UInt16:  return 1;     break;
            case DataType::Int32:     return 1;     break;
            case DataType::UInt32:    return 1;     break;
            case DataType::Float:   return 1;     break;
            case DataType::Float2:  return 2;     break;
            case DataType::Float3:  return 3;     break;
            case DataType::Float4:  return 4;     break;
            case DataType::Matrix3: return 3 * 3; break;
            case DataType::Matrix4: return 4 * 4; break;
        }
    }
}