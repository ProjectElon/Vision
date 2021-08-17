#pragma once

#include <iostream>
#include <cstring>
#include <string>

#include "Vision/Core/Defines.h"
#include "Vision/Platform/Memory.h"

namespace Vision
{
    introspect_struct("data structures", false)
    struct String
    {
        char*  Data  = nullptr;
        size_t Count = 0;
    };

    String MakeString(const char* cstring, size_t count);
    String MakeString(const char* cstring);

    String CopyString(const String* string);
    String CopyString(const String& string);

    void AssignString(String* to, String* from);
    void AssignString(String* to, const char* cstring);
    void AssignString(String* to, const char* cstring, size_t count);

    void AssignString(String& to, String& from);
    void AssignString(String& to, const char* cstring);
    void AssignString(String& to, const char* cstring, size_t count);

    void FreeString(String* string);
    void FreeString(String& string);

    bool operator==(const String& lhs, const String& rhs);
    bool operator==(const String& lhs, const char* rhs);
    bool operator==(const char* lhs, const String& rhs);

    bool operator!=(const String& lhs, const String& rhs);
    bool operator!=(const String& lhs, const char* rhs);
    bool operator!=(const char* lhs, const String& rhs);

    inline std::ostream& operator<<(std::ostream& os, const String& string)
    {
        os << string.Data;
        return os;
    }
}