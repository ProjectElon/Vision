#include "pch.h"
#include "Vision/Platform/EntryPoint.h"
#include "Vision/Core/Application.h"

extern Vision::Application* Vision::CreateApplication();

#ifdef VN_PLATFORM_DESKTOP

	int main(int argCount, char* args[])
	{
        using namespace Vision;

		Log::Init();

		Application* application = CreateApplication();
		application->Run();

		delete application;

		return 0;
	}

#endif