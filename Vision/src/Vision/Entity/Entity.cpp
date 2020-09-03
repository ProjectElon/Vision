#include "pch.h"
#include "Vision/Entity/Entity.h"

namespace Vision
{
	Entity::Entity(EntityHandle handle, Scene* scene)
		: m_Handle(handle)
		, m_Scene(scene)
	{
	}
}