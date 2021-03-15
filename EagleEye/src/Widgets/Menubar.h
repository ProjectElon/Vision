#pragma once

namespace Vision
{
	class EditorLayer;

	class Menubar
	{
	public:
		EditorLayer* Editor;

		Menubar() = default;
		Menubar(EditorLayer* editor);

		void OnImGuiRender();
	};
}