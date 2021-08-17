#include "pch.hpp"
#include "Vision/Entity/Components.h"

namespace Vision
{
    void ComponentStorage::ReConstruct(uint32 MaxEntityCount, uint32 alignment)
    {
        uint8* newData = (uint8*)_mm_malloc(MaxEntityCount * (memorysize)SizeInBytes, alignment);
        uint32* newEntities = new uint32[MaxEntityCount];
        Alignment = alignment;

        if (Data)
        {
            memcpy(newData, Data, Count * (memorysize)SizeInBytes);
            memcpy(newEntities, Entities, Count * sizeof(uint32));

            _mm_free(Data);
            delete[] Entities;
        }

        Data = newData;
        Entities = newEntities;
    }

    ComponentStorage::~ComponentStorage()
    {
        _mm_free(Data);

        delete[] Entities;
    }
}