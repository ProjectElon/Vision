#include "pch.h"
#include "Input.h"
#include "Vision/Core/Core.h"

#if VN_PLATFORM_DESKTOP
	#include "Vision/Platform/Desktop/DesktopInput.h"
#endif

namespace Vision
{
	Input* Input::s_Instance = nullptr;

	void Input::Initialize()
	{
	#if VN_PLATFORM_DESKTOP
		s_Instance = new DesktopInput();
	#endif
	}

	void Input::ShutDown()
	{
		delete s_Instance;
	}
}