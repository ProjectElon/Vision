#pragma once

#include <Vision.h>

class Editor : public Vision::Application
{
public:
	Editor();
	~Editor();
};

RegisterEntryPointClass(Editor)