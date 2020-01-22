#include "pch.h"

#include "Layer.h"

namespace Vision
{
	Layer::Layer(const std::string& name, bool enabled)
		: m_Name(name)
		, m_Enabled(enabled)
	{
	}

	void Layer::Enable()
	{
		if (!m_Enabled)
		{
			m_Enabled = true;
			OnEnable();
		}
	}

	void Layer::Disable()
	{
		if (m_Enabled)
		{
			m_Enabled = false;
			OnDisable();
		}
	}
}