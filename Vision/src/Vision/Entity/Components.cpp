#include "pch.h"
#include "Vision/Entity/Components.h"
#include "Vision/Renderer/Texture2D.h"

namespace Vision
{
    void ComponentStorage::ReConstruct(uint32 MaxEntityCount)
    {
        uint8* newData = new uint8[MaxEntityCount * SizeInBytes];
        uint32* newEntities = new uint32[MaxEntityCount];

        if (Data)
        {
            memcpy(newData, Data, Count * SizeInBytes);
            memcpy(newEntities, Entities, Count * sizeof(uint32));

            delete[] Data;
            delete[] Entities;
        }

        Data = newData;
        Entities = newEntities;
    }

    ComponentStorage::~ComponentStorage()
    {
        delete[] Data;
        delete[] Entities;
    }
}