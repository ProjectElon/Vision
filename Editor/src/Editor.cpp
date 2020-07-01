#include "Editor.h"
#include "EditorLayer.h"

Editor::Editor()
{
	PushLayer(new EditorLayer);
}

Editor::~Editor()
{
}