#pragma once

#include "Vision/Core/Layer.h"

#include <vector>

namespace Vision
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack() = default;

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		Layer* FindLayerByName(const std::string& name) const;
		bool RemoveLayerByName(const std::string& name);

		auto begin()  { return Layers.begin();  }
		auto end()    { return Layers.end();    }
		auto rbegin() { return Layers.rbegin(); }
		auto rend()   { return Layers.rend();   }

		auto begin()  const { return Layers.begin();  }
		auto end()    const { return Layers.end();    }
		auto rbegin() const { return Layers.rbegin(); }
		auto rend()   const { return Layers.rend();   }

	private:
		std::vector<Layer*> Layers;
		uint32 InsertIndex = 0;
	};
}