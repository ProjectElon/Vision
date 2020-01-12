#include "Sandbox.h"

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

Vision::Application* Vision::CreateApplication()
{
	return new Sandbox();
}