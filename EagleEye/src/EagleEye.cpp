#include "EagleEye.h"
#include "EditorLayer.h"

namespace Vision
{
	EagleEye::EagleEye()
	{
		PushLayer(new EditorLayer);
	}

	EagleEye::~EagleEye()
	{
	}
}