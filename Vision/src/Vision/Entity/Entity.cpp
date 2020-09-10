#include "pch.h"
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
	Entity::Entity()
		: m_Handle(0)
		, m_Scene(nullptr)
	{
	}

	Entity::Entity(EntityHandle handle, Scene* scene)
		: m_Handle(handle)
		, m_Scene(scene)
	{
	}

	bool Entity::Isvalid()
	{
		if (m_Scene != nullptr)
		{
			auto entityIter =  m_Scene->m_Entites.find(m_Handle);
			
			if (entityIter != m_Scene->m_Entites.end())
			{
				return true;
			}
		}

		return false;
	}

	const EntityStorage& Entity::GetStorage()
	{
		return m_Scene->GetEntityStorage(m_Handle);
	}

	void* Entity::GetComponent(ComponentID componentID, ComponentIndex componentIndex)
	{
		return m_Scene->GetComponent(m_Handle, componentID, componentIndex);
	}
}