#pragma once

#include <Vision.h>

namespace Vision
{
	class Editor : public Application
	{
	public:
		Editor();
		~Editor();
	};
}

RegisterEntryPointClass(Editor)