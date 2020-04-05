#pragma once

#include "pch.h"
#include "Vision/Core/Core.h"

namespace Vision
{ 
	namespace API
	{
		enum class DataType : uint8_t
		{
			Bool,
			Byte,
			UByte,
			Short,
			UShort,
			Int,
			UInt,
			Float,
			Float2,
			Float3,
			Float4,
			Matrix3,
			Matrix4
		};

		enum ClearFlag : uint32_t
		{
			ClearColorBufferBit   = VN_BIT(1),
			ClearDepthBufferBit   = VN_BIT(2),
			ClearStencilBufferBit = VN_BIT(3)
		};

		uint32_t GetDataTypeSize(DataType dataType);
		uint32_t GetDataTypeComponentCount(DataType dataType);
	}
}