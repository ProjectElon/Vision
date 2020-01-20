#include "pch.h"

#include "LayerStack.h"

namespace Vision
{
	LayerStack::LayerStack()
		: m_LayerInsertIndex(0)
	{
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	Layer* LayerStack::FindLayerByName(const std::string& name) const
	{
		for (auto it = begin(); it != end(); ++it)
		{
			if ((*it)->GetName() == name)
			{
				return (*it);
			}
		}

		return nullptr;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(begin() + m_LayerInsertIndex, layer);
	}

	void LayerStack::PopLayer(Layer* layer) 
	{
		auto it = std::find(begin(), begin() + m_LayerInsertIndex, layer);
		
		if (it != begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}
	
	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(begin() + m_LayerInsertIndex, end(), overlay);
		
		if (it != end())
		{
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}
}