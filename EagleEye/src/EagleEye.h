#pragma once

#include "Vision/Core/Application.h"
#include "Vision/Platform/EntryPoint.h"

#include "EditorState.h"

namespace Vision
{
	class EagleEye : public Application
	{
	public:
		EagleEye();
		~EagleEye();

		static EditorState EditorState;
	};
}