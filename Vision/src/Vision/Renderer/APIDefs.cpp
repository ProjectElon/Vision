#include "pch.h"
#include "Vision/Renderer/APIDefs.h"

namespace Vision
{
	namespace API
	{
		uint32_t GetDataTypeSize(DataType APIDataType)
		{
			switch (APIDataType)
			{
				case DataType::Bool:  { return 1; } break;
				case DataType::Byte:  { return 1; } break;
				case DataType::UByte: { return 1; } break;

				case DataType::Short:  { return 2; } break;
				case DataType::UShort: { return 2; } break;

				case DataType::Int:  { return 4; } break;
				case DataType::UInt: { return 4; } break;

				case DataType::Float:  { return 1 * 4; } break;
				case DataType::Float2: { return 2 * 4; } break;
				case DataType::Float3: { return 3 * 4; } break;
				case DataType::Float4: { return 4 * 4; } break;

				case DataType::Matrix3: { return 3 * 3 * 4; } break;
				case DataType::Matrix4: { return 4 * 4 * 4; } break;
			}
		}

		uint32_t GetDataTypeComponentCount(DataType APIDataType)
		{
			switch (APIDataType)
			{
				case DataType::Bool:  { return 1; } break;
				case DataType::Byte:  { return 1; } break;
				case DataType::UByte: { return 1; } break;

				case DataType::Short:  { return 1; } break;
				case DataType::UShort: { return 1; } break;

				case DataType::Int:  { return 1; } break;
				case DataType::UInt: { return 1; } break;

				case DataType::Float:  { return 1; } break;
				case DataType::Float2: { return 2; } break;
				case DataType::Float3: { return 3; } break;
				case DataType::Float4: { return 4; } break;

				case DataType::Matrix3: { return 3 * 3; } break;
				case DataType::Matrix4: { return 4 * 4; } break;
			}
		}
	}
}