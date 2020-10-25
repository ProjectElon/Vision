#include <Vision.h>

namespace Vision
{
	class Scene;

	class Menubar
	{
	public:
		void OnImGuiRender();
		void OnEvent(Event& e);

	private:
		void NewScene();
		void OpenScene();
		void SaveSceneAs(Scene& scene);
		void CloseScene();

		bool OnKeyPressed(KeyPressedEvent& e);
	};
}