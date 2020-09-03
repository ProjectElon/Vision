#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Entity.h"

#define MAX_COMPONENT_SIZE 1000
#define STARTING_COMPONENT_BUFFER_SIZE 100

namespace Vision
{
	using ComponentID = uint32;
    using ComponentIndex = uint32;

    template <typename... Args>
    struct TypeList
    {
        template <std::size_t N>
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    struct TagComponent
    {
        std::string Tag;
    };
}