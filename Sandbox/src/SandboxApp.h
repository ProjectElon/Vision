#pragma once

#include <Vision.h>

class SandboxApplication : public Vision::Application
{
public:
	SandboxApplication();
	~SandboxApplication();
};

RegisterEntryPointClass(SandboxApplication)