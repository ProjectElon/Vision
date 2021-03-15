#include "pch.h"

#include "LayerStack.h"

namespace Vision
{
	Layer* LayerStack::FindLayerByName(const std::string& name) const
	{
		for (auto layerIt = begin(); layerIt != end(); ++layerIt)
		{
			if ((*layerIt)->Name == name)
			{
				return (*layerIt);
			}
		}

		return nullptr;
	}

	bool LayerStack::RemoveLayerByName(const std::string& name)
	{
		for (auto layerIt = begin(); layerIt != end(); ++layerIt)
		{
			if ((*layerIt)->Name == name)
			{
				Layers.erase(layerIt);
				return true;
			}
		}

		return false;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		Layers.emplace(begin() + InsertIndex, layer);
	}

	void LayerStack::PopLayer(Layer* layer) 
	{
		auto layerIt = std::find(begin(), begin() + InsertIndex, layer);
		
		if (layerIt != begin() + InsertIndex)
		{
			layer->OnDetach();
			Layers.erase(layerIt);
			InsertIndex--;
		}
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		Layers.emplace_back(overlay);
	}
	
	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto LayerIt = std::find(begin() + InsertIndex, end(), overlay);
		
		if (LayerIt != end())
		{
			overlay->OnDetach();
			Layers.erase(LayerIt);
		}
	}
}