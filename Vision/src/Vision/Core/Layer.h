#pragma once

#include "Vision/Core/Core.h"
#include "Vision/Events/Event.h"

namespace Vision
{
	class Layer
	{
	public:
		Layer(const std::string& name);

		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float dt) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_Name; }

	protected:
		std::string m_Name;
	};
}