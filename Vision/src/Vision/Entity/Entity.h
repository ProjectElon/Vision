#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
	namespace Entity
	{
		static union
		{
			uint32 null = 0;
		};
	}

	using EntityHandle  = uint32;
	using EntityStorage = std::unordered_map<ComponentID, ComponentIndex>;
}