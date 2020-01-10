#include <Vision.h>

class Sandbox : public Vision::Application
{
public:
	Sandbox()
	{
	}

	~Sandbox()
	{
	}
};

Vision::Application* Vision::CreateApplication()
{
	return new Sandbox();
}