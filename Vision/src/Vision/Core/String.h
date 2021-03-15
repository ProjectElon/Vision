#pragma once

#include <iostream>
#include <cstring>
#include <string>

#include "Vision/Core/Common.h"

namespace Vision
{
    Inspect(category : "data structures", handle : false)
    struct String
    {
        char*  Text  = nullptr;
        uint32 Count = 0;

        Ignore()

        String() = default;

        String(const char* cstring)
        {
            uint32 count = strlen(cstring);
            Init(cstring, count);
        }

        String(const char* cstring, uint32 count)
        {
            Init(cstring, count);
        }

        String(const String& other)
        {
            Init(other.Text, other.Count);
        }

        String(String&& other)
        {
            Text = other.Text;
            Count = other.Count;

            other.Text = nullptr;
            other.Count = 0;
        }

        ~String()
        {
            Uninit();
        }

        String& operator=(const String& rhs)
        {
            Uninit();
            Init(rhs.Text, rhs.Count);
            return *this;
        }

        String& operator=(String&& rhs)
        {
            if (this != &rhs)
            {
                Uninit();
                Text = rhs.Text;
                Count = rhs.Count;
                rhs.Text = nullptr;
                rhs.Count = 0;
            }

            return *this;
        }

        void Init(const char* cstring, uint32 count)
        {
            if (count)
            {
                Text = new char[count + 1];
                Count = count;

                memcpy(Text, cstring, count + 1);
            }
        }

        void Uninit()
        {
            if (Text)
            {
                delete[] Text;

                Text = nullptr;
                Count = 0;
            }
        }

        bool operator==(const String& rhs)
        {
            if (Count != rhs.Count)
            {
                return false;
            }

            for (uint32 charIndex = 0; charIndex < Count; ++charIndex)
            {
                if (Text[charIndex] != rhs.Text[charIndex])
                {
                    return false;
                }
            }

            return true;
        }

        inline bool operator!=(const String& rhs)
        {
            return !operator==(rhs);
        }

        inline bool IsEmpty()
        {
            return Text == nullptr;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const String& string)
    {
        os << string.Text;
        return os;
    }
}