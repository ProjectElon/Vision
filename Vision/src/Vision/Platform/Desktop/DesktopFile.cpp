#include "pch.h"

#ifdef  VN_PLATFORM_DESKTOP

#include "Vision/IO/File.h"
#include <cstdio>

namespace Vision
{
    static const char* s_FileModeTable[(uint32)FileMode::Count] =
    {
        "r",
        "w",
        "a",
        "w+"
    };

    static const char* s_FileModeBinaryTable[(uint32)FileMode::Count] =
    {
        "rb",
        "wb",
        "ab",
        "wb+"
    };

    FileStream File::Open(const std::string& filepath, FileMode fileMode, bool binary)
    {
        FileStream stream;

        const char* mode = (binary) ? s_FileModeBinaryTable[(uint32)fileMode] : s_FileModeTable[(uint32)fileMode];
        stream.Handle = fopen(filepath.c_str(), mode);

        if (stream.Handle)
        {
            fseek((FILE*)stream.Handle, 0, SEEK_END);
            stream.SizeInBytes = ftell((FILE*)stream.Handle);
            fseek((FILE*)stream.Handle, 0, SEEK_SET);
        }

        return stream;
    }

    uint32 File::Read(const FileStream& fileStream, void* dest, uint32 sizeInBytes)
    {
        return fread(dest, 1, sizeInBytes, (FILE*)fileStream.Handle);
    }

    uint32 File::Write(const FileStream& fileStream, const void* data, uint32 sizeInBytes)
    {
        return fwrite(data, 1, sizeInBytes, (FILE*)fileStream.Handle);
    }

    std::string File::ReadContents(const FileStream& fileStream)
    {
        std::string contents;
        contents.resize(fileStream.SizeInBytes);
        Read(fileStream, contents.data(), fileStream.SizeInBytes);
        return contents;
    }

    void File::WriteContents(const FileStream& fileStream, const std::string& contents)
    {
        Write(fileStream, contents.data(), contents.length());
    }

    bool File::ReadLine(const FileStream& fileStream, std::string& line, uint32 chunkSize)
    {
        line.clear();

        uint32 chunkCount = 1;

        while (true)
        {
            line.resize(chunkCount * chunkSize);
            char* chunk = line.data() + (chunkCount - 1) * chunkSize;
            memset(chunk, 0, chunkSize);

            if (fgets(chunk, chunkSize + 1, (FILE*)fileStream.Handle))
            {
                if (chunk[chunkSize - 1] == '\0' || chunk[chunkSize - 1] == '\n')
                {
                    break;
                }
            }
            else
            { 
                if (chunkCount == 1)
                {
                    return false;
                }
                else
                {
                    break;
                }
            }

            ++chunkCount;
        }

        while (line.back() == '\0' || line.back() == '\n')
        {
            line.pop_back();
        }

        return true;
    }

    uint32 File::GetPosition(const FileStream& fileStream)
    {
        return ftell((FILE*)fileStream.Handle);
    }

    void File::Seek(const FileStream& fileStream, uint32 position)
    {
        fseek((FILE*)fileStream.Handle, position, SEEK_SET);
    }

    void File::Close(const FileStream& fileStream)
    {
        if (fileStream.Handle)
        {
            fclose((FILE*)fileStream.Handle);
        }
    }
}

#endif