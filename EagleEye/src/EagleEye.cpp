#include "EagleEye.h"
#include "EditorLayer.h"

namespace Vision
{
	EagleEye::EagleEye()
	{
		EditorLayer* editorLayer = new EditorLayer;
		PushLayer(editorLayer);
	}

	EagleEye::~EagleEye()
	{
	}
}