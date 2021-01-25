#pragma once

namespace Vision
{
	class EditorLayer;
	
	class Menubar
	{
	public:
		void SetEditor(EditorLayer* editor);
		void OnImGuiRender();

	private:
		EditorLayer* m_Editor;
	};
}