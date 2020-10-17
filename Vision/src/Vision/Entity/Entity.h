#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
	using Entity = uint32;
	
	namespace entity
	{
		static union
		{
			uint32 null = 0;
		};
	}

	using EntityStorage = std::unordered_map<ComponentID, ComponentIndex>;
}