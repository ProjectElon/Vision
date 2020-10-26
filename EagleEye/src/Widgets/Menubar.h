#include <Vision.h>

namespace Vision
{
	class Scene;

	class Menubar
	{
	public:
		void OnImGuiRender();
		
		bool OnKeyPressed(KeyPressedEvent& e);

	private:
		void NewScene(const std::string& filepath, uint32 maxEntityCount);
		void OpenScene();
		void SaveSceneAs(Scene& scene);
		void SaveScene(Scene& scene);
		void CloseScene(Scene& scene);

		std::string m_Action;
	};
}