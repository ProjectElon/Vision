#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Log.h"
#include "Vision/Core/String.h"

namespace Vision
{
    String MakeString(const char* cstring, size_t count)
    {
        VnCoreAssert(cstring != nullptr);
        VnCoreAssert(strlen(cstring) == count);

        String string;

        if (count > 0)
        {
            string.Count = count;
            string.Data  = VnAllocateStructArray(char, count + 1);
            memcpy(string.Data, cstring, sizeof(char) * (count + 1));
        }

        return string;
    }

    String MakeString(const char* cstring)
    {
        size_t count = strlen(cstring);
        String string = MakeString(cstring, count);
        return string;
    }

    String CopyString(const String* string)
    {
        const char* cstring = static_cast<const char*>(string->Data);
        return MakeString(cstring, string->Count);
    }

    String CopyString(const String& string)
    {
        const char* cstring = static_cast<const char*>(string.Data);
        return MakeString(cstring, string.Count);
    }

    void AssignString(String* to, String* from)
    {
        VnCoreAssert(to != from, "can't copy from a freed address");
        FreeString(to);
        to->Count = from->Count;
        to->Data = VnAllocateStructArray(char, from->Count + 1);
        memcpy(to->Data, from->Data, sizeof(char) * (from->Count + 1));
    }

    void AssignString(String* string, const char* cstring)
    {
        VnCoreAssert(string->Data != cstring, "can't copy from a freed address");
        FreeString(string);
        *string = MakeString(cstring);
    }

    void AssignString(String* string, const char* cstring, size_t count)
    {
        FreeString(string);
        *string = MakeString(cstring, count);
    }

    void AssignString(String& to, String& from)
    {
        AssignString(&to, &from);
    }

    void AssignString(String& string, const char* cstring)
    {
        AssignString(&string, cstring);
    }

    void AssignString(String& string, const char* cstring, size_t count)
    {
        AssignString(&string, cstring, count);
    }

    void FreeString(String* string)
    {
        VnCoreAssert(string != nullptr);
        
        VnFree(string->Data);

#ifndef VN_DIST
        string->Data  = nullptr;
        string->Count = 0;
#endif
    }

    void FreeString(String& string)
    {
        FreeString(&string);
    }

    bool operator==(const String& lhs, const String& rhs)
    {
        if (lhs.Count != rhs.Count)
        {
            return false;
        }

        for (uint32 index = 0; index < lhs.Count; ++index)
        {
            if (lhs.Data[index] != rhs.Data[index])
            {
                return false;
            }
        }

        return true;
    }

    bool operator==(const String& lhs, const char* rhs)
    {
        size_t count = strlen(rhs);

        if (lhs.Count != count)
        {
            return false;
        }

        for (uint32 index = 0; index < count; ++index)
        {
            if (lhs.Data[index] != rhs[index])
            {
                return false;
            }
        }

        return true;
    }

    bool operator==(const char* lhs, const String& rhs)
    {
        return operator==(rhs, lhs);
    }

    bool operator!=(const String& lhs, const String& rhs)
    {
        return !(operator==(lhs, rhs));
    }

    bool operator!=(const String& lhs, const char* rhs)
    {
        return !(operator==(lhs, rhs));
    }

    bool operator!=(const char* lhs, const String& rhs)
    {
        return !(operator==(rhs, lhs));
    }
}