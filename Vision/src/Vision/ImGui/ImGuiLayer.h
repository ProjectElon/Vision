#pragma once

#include "Vision/Core/Layer.h"
#include "Vision/Platform/Window.h"

namespace Vision
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		Window* m_Window;
	};
}