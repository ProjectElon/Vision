#include "SandboxApp.h"
#include "GameLayer.h"

Sandbox::Sandbox()
{
	PushLayer(new GameLayer);
}

Sandbox::~Sandbox()
{
}

Vision::Application* Vision::CreateApplication()
{
	return new Sandbox();
}