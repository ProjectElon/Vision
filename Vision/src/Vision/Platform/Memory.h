#pragma once

#include "Vision/Core/Common.h"

#define KiloBytes(Count) (memorysize)(Count)            * (memorysize)1024
#define MegaBytes(Count) (memorysize)KiloBytes((Count)) * (memorysize)1024
#define GigaBytes(Count) (memorysize)MegaBytes((Count)) * (memorysize)1024

namespace Vision
{
    template<typename T>
    struct MemoryPool
    {
        T* BasePointer = nullptr;
        uint32 MaxElementCount = 0;
        uint32 ElementCount = 0;

        MemoryPool() = default;

        MemoryPool(uint32 maxElementCount)
        {
            Init(maxElementCount);
        }

        ~MemoryPool()
        {
            Uninit();
        }

        void Init(uint32 maxElementCount)
        {
            BasePointer = reinterpret_cast<T*>(_mm_malloc((memorysize)maxElementCount * sizeof(T), alignof(T)));
            MaxElementCount = maxElementCount;
        }

        void Uninit()
        {
            VnCoreAssert(BasePointer != nullptr, "BasePointer is nullptr");
            _mm_free(BasePointer);
        }

        inline T* Allocate()
        {
            VnCoreAssert(ElementCount < MaxElementCount, "out of bounds");

            T* elementPointer = BasePointer + ElementCount;
            ElementCount++;
            return elementPointer;
        }

        inline T* AllocateAndAssign(const T& element)
        {
            T* elementPointer = Allocate();
            memcpy(elementPointer, &element, sizeof(T));
            return elementPointer;
        }

        inline T& GetElement(uint32 index)
        {
            VnCoreAssert(index < ElementCount, "out of bounds");
            return *(BasePointer + index);
        }

        inline T& operator[](uint32 index)
        {
            return GetElement(index);
        }
    };

    template<typename T>
    struct FreeList
    {
        struct FreeListNode
        {
            T      Element;
            uint32 Next;
        };

        FreeListNode* Nodes = nullptr;
        uint32 MaxElementCount = 0;
        uint32 ElementCount = 0;
        uint32 HeadNode = 0;

        FreeList() = default;

        FreeList(uint32 maxElementCount)
        {
            Init(maxElementCount);
        }

        ~FreeList()
        {
            Uninit();
        }

        void Init(uint32 maxElementCount)
        {
            Nodes = reinterpret_cast<FreeListNode*>(_mm_malloc((memorysize)maxElementCount * sizeof(FreeListNode), alignof(FreeListNode)));
            MaxElementCount = maxElementCount;

            for (uint32 nodeIndex = 0; nodeIndex < MaxElementCount; ++nodeIndex)
            {
                Nodes[nodeIndex].Next = nodeIndex == MaxElementCount - 1 ?
                                        MaxElementCount : (nodeIndex + 1);
            }
        }

        void Uninit()
        {
            VnCoreAssert(Nodes != nullptr, "Nodes is nullptr");
            _mm_free(Nodes);
            Nodes = nullptr;
        }

        T* Allocate()
        {
            VnCoreAssert(ElementCount < MaxElementCount, "out of bounds");

            T* elementPointer = reinterpret_cast<T*>(Nodes + HeadNode);
            ElementCount++;

            if (ElementCount != MaxElementCount)
            {
                uint32 nextHead = HeadNode;
                nextHead = Nodes[HeadNode].Next;
                HeadNode = nextHead;
            }
            else
            {
                HeadNode = MaxElementCount;
            }

            return elementPointer;
        }

        T* AllocateAndAssign(const T& element)
        {
            T* elementPointer = Allocate();
            memcpy(elementPointer, &element, sizeof(T));
            return elementPointer;
        }

        void Deallocate(T* elementPointer)
        {
            VnCoreAssert(ElementCount > 0, "list is already empty");
            VnCoreAssert(elementPointer != nullptr, "elementPointer is nullptr");

            uint32 elementIndex = ((FreeListNode*)elementPointer) - Nodes;

            VnCoreAssert(elementIndex < MaxElementCount, "out of bounds");

            ElementCount--;

            Nodes[elementIndex].Next = HeadNode;
            HeadNode = elementIndex;
        }
    };

#define RewindScope(stackAllocator) StackAllocator::StackScope vision__##__LINE__##scope_stack(stackAllocator);

    struct StackAllocator
    {
        struct StackScope
        {
            uint8*          StackPointer = nullptr;
            StackAllocator* Stack = nullptr;

            StackScope(StackAllocator* stack)
                : Stack(stack),
                StackPointer(stack->StackPointer)
            {
            }

            ~StackScope()
            {
                Stack->StackPointer = StackPointer;
            }
        };

        uint8* StackBase = nullptr;
        uint8* StackPointer = nullptr;
        memorysize SizeInBytes = 0;

        StackAllocator() = default;

        StackAllocator(memorysize sizeInBytes)
        {
            Init(sizeInBytes);
        }

        ~StackAllocator()
        {
            Uninit();
        }

        void Init(memorysize sizeInBytes)
        {
            SizeInBytes  = sizeInBytes;
            StackBase    = reinterpret_cast<uint8*>(malloc(sizeInBytes));
            StackPointer = StackBase;
        }

        void Uninit()
        {
            free(StackBase);
        }

        template<typename T>
        T* Allocate()
        {
            // @(Harlequin): can we get rid of the remainder !
            memorysize remainder = reinterpret_cast<memorysize>(StackPointer) % alignof(T);
            VnCoreAssert(alignof(T) < 256, "we only store the offset in one byte");
            VnCoreAssert((StackPointer + remainder + sizeof(T)) - StackBase <= SizeInBytes, "stack overflow");

            StackPointer += remainder;
            T* result = reinterpret_cast<T*>(StackPointer);
            StackPointer += sizeof(T);
            uint8* offset = reinterpret_cast<uint8*>(StackPointer);
            *offset = static_cast<uint8>(remainder);
            StackPointer += 1;
            return result;
        }

        template<typename T>
        inline T* AllocateAndAssign(const T& value)
        {
            T* result = Allocate<T>();
            memcpy(result, &value, sizeof(T));
            return result;
        }

        template<typename T>
        void Deallocate(T* address)
        {
            VnCoreAssert((uint8*)(address) >= StackBase && (uint8*)(address) < StackPointer, "invalid address");
            uint8 offset = *((uint8*)(address) + sizeof(T));
            StackPointer = (uint8*)(address) - offset;
        }

        void Rewind()
        {
            StackPointer = StackBase;
        }
    };
}