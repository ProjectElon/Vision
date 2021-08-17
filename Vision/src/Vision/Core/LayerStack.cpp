#include "pch.hpp"

#include "LayerStack.h"

namespace Vision
{
	Layer* LayerStack::FindLayerByName(const std::string& name) const
	{
		for (auto layerIt = Layers.begin(); layerIt != Layers.end(); ++layerIt)
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
		for (auto layerIt = Layers.begin(); layerIt != Layers.end(); ++layerIt)
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
		Layers.emplace(Layers.begin() + InsertIndex, layer);
	}

	void LayerStack::PopLayer(Layer* layer) 
	{
		auto layerIt = std::find(Layers.begin(), Layers.begin() + InsertIndex, layer);
		
		if (layerIt != Layers.begin() + InsertIndex)
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
		auto LayerIt = std::find(Layers.begin() + InsertIndex, Layers.end(), overlay);
		
		if (LayerIt != Layers.end())
		{
			overlay->OnDetach();
			Layers.erase(LayerIt);
		}
	}
}