#pragma once

#include "Vision/Core/Defines.h"

#include <string>

namespace Vision
{
    enum class FileMode
    {
        Read,
        Write,
        Append,
        ReadAndWrite,
        Count
    };

    struct FileStream
    {
        void*  Handle = nullptr;
        uint32 SizeInBytes = 0;

        bool IsOpen() const
        {
            return Handle != nullptr;
        }
    };

    class File
    {
    public:
        static FileStream Open(const std::string& filepath, FileMode fileMode, bool binary = false);

        static uint32 Read(const FileStream& fileStream, void* dest, uint32 sizeInBytes);
        static uint32 Write(const FileStream& fileStream, const void* data, uint32 sizeInBytes);

        static std::string ReadContents(const FileStream& fileStream);
        static void WriteContents(const FileStream& fileStream, const std::string& contents);

        // @(Harlequin): Is there is a way to set a good value for chuckSize ?
        static bool ReadLine(const FileStream& fileStream, std::string& line, uint32 chunkSize = 1024);

        static void Seek(const FileStream& fileStream, uint32 position);
        static uint32 GetPosition(const FileStream& fileStream);
        static void Reset(const FileStream& fileStream);

        static void Close(FileStream& fileStream);
    };
}