#pragma once

#include "Vision/Core/Core.h"
#include "Vision/Core/Log.h"
#include "Vision/Core/Application.h"

extern Vision::Application* Vision::CreateApplication();

#ifdef VN_PLATFORM_WINDOWS

	int main(int argc, char** argv)
	{
		Vision::Log::Initialize();

		auto app = Vision::CreateApplication();
		app->Run();
		delete app;

		return 0;
	}

#endif