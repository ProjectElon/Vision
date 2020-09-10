#pragma once

#include <Vision.h>
#include <rapidjson/document.h>

namespace Vision
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(float deltaTime) override;
		void OnEvent(Vision::Event& e) override;
		void OnImGuiRender() override;

		template<typename Component>
		void InspectComponent()
		{
			auto& componentTypes = Scene::GetComponenetTypes();
			
			auto it = componentTypes.find(std::type_index(typeid(Component)));
			
			ComponentID componentID;

			if (it != componentTypes.end())
			{
				componentID = it->second;
			}
			else
			{
				componentID = Scene::GenerateComponentID();
				componentTypes.emplace(std::type_index(typeid(Component)), componentID);
			}

			m_ComponentInspectors[componentID] = ShowInInspectorFn<Component>;
		}

	private:
		void LoadSettings();
		void SaveSettings();

	private:
		Window* m_Window;

		Vision::Scope<OrthographicCameraController> m_CameraController;

		Vision::Ref<Shader> m_SpriteShader;

		Vision::Ref<Texture2D> m_CheckboardTexture;
		Vision::Ref<Texture2D> m_PlayerTexture;

		SpriteComponent m_CheckerboardSprite;
		
		glm::vec4 m_WhiteColor;

		glm::vec4 m_ClearColor = {};
		glm::vec4 m_LastClearColor = {};

		glm::vec2 m_ViewportSize = { 800.0f, 600.0f };
		Ref<FrameBuffer> m_SceneFrameBuffer;

		bool m_IsViewportFocused = false;
		bool m_IsViewportHovered = false;
		bool m_LastIsVSyncEnabled = false;
		bool m_IsVSyncEnabled = false;

		rapidjson::Document m_Settings;

		std::unique_ptr<Scene> m_MainScene;
		Entity m_SelectedEntity;

		std::unordered_map<ComponentID, std::function<void(void*)>> m_ComponentInspectors;
		bool m_UseMainGameCamera = false;
	};
}