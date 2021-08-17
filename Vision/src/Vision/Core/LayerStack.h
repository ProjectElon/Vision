#pragma once

#include "Vision/Core/Layer.h"

#include <vector>

namespace Vision
{
	class LayerStack
	{
	public:
		std::vector<Layer*> Layers;
		uint32 InsertIndex = 0;

		LayerStack() = default;
		~LayerStack() = default;

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		Layer* FindLayerByName(const std::string& name) const;
		bool RemoveLayerByName(const std::string& name);
	};
}