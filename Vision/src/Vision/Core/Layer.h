#pragma once

#include "Vision/Core/Core.h"
#include "Vision/Events/Event.h"

namespace Vision
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer", bool enabled = true);

		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEnable() {}
		virtual void OnDisable() {}

		virtual void OnUpdate(float dt) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		void Enable();
		void Disable();

		inline const std::string& GetName() const { return m_Name; }
		inline bool IsEnabled() const { return m_Enabled; }

	protected:
		std::string m_Name;
		bool m_Enabled;
	};
}