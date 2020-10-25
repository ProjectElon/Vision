#pragma once

#include <Vision.h>

namespace Vision
{
	class EagleEye : public Application
	{
	public:
		EagleEye();
		~EagleEye();
	};
}

RegisterEntryPointClass(EagleEye)