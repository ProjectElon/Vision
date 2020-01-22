#include "pch.h"
#include "Vision/Core/Core.h"

#if VN_PLATFORM_DESKTOP
	
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include <examples/imgui_impl_opengl3.cpp>
	#include <examples/imgui_impl_glfw.cpp>
	
#endif