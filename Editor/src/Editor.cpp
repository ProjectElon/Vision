#include "Editor.h"
#include "EditorLayer.h"

namespace Vision
{
	Editor::Editor()
	{
		PushLayer(new EditorLayer);
	}

	Editor::~Editor()
	{
	}
}