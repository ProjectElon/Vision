#pragma once

#include "Vision/Core/Layer.h"

#include <vector>

namespace Vision
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		Layer* FindLayerByName(const std::string& name) const;
		bool RemoveLayerByName(const std::string& name);

		auto begin()  { return m_Layers.begin();  }
		auto end()    { return m_Layers.end();    }
		auto rbegin() { return m_Layers.rbegin(); }
		auto rend()   { return m_Layers.rend();   }

		auto begin()  const { return m_Layers.begin();  }
		auto end()    const { return m_Layers.end();    }
		auto rbegin() const { return m_Layers.rbegin(); }
		auto rend()   const { return m_Layers.rend();   }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex;
	};
}