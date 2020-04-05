#include "SandboxApp.h"
#include "GameLayer.h"

SandboxApplication::SandboxApplication()
{
	PushLayer(new GameLayer);
}

SandboxApplication::~SandboxApplication()
{
}