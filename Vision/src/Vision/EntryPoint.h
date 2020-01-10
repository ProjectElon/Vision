#pragma once

#include <stdio.h>

#include "Application.h"

#ifdef VN_PLATFORM_WINDOWS

extern Vision::Application* Vision::CreateApplication();

int main(int argc, char** argv)
{
	printf("Hello, World from Vision !");

	auto app = Vision::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif