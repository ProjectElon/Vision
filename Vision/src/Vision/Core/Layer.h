#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Events/Event.h"

namespace Vision
{
	class Layer
	{
	public:
		std::string Name;

		Layer() = default;
		Layer(const std::string& name);

		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float dt) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	};
}