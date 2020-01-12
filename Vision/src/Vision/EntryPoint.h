#pragma once

#include "Application.h"
#include "Log.h"

#ifdef VN_PLATFORM_WINDOWS

extern Vision::Application* Vision::CreateApplication();

int main(int argc, char** argv)
{
	Vision::Log::Initialize();

	auto app = Vision::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif