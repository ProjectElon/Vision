#pragma once

namespace Vision
{
	class EditorLayer;
	class Scene;

	class Menubar
	{
	public:
		void SetEditorLayer(EditorLayer* editor);
		void OnImGuiRender();
		
	private:
		EditorLayer* m_EditorLayer;
	};
}