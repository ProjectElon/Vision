#include "pch.h"
#include "Input.h"
#include "Vision/Core/Core.h"

#if VN_PLATFORM_DESKTOP
	#include "Vision/Platform/Desktop/DesktopInput.h"
#endif

namespace Vision
{
	Scope<Input> Input::s_Instance;

	void Input::Init()
	{
	#if VN_PLATFORM_DESKTOP
		s_Instance = CreateScope<DesktopInput>();
	#endif
	}

	void Input::ShutDown()
	{
		s_Instance.release();
	}
}