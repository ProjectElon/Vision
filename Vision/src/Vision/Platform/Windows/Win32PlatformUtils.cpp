#include "pch.h"
#include "Vision/Core/Base.h"

#ifdef VN_PLATFORM_WINDOWS

#include <commdlg.h>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Vision/Platform/PlatformUtils.h"
#include "Vision/Core/Application.h"
#include "Vision/IO/FileSystem.h"

namespace Vision
{
    std::string FileDialog::OpenFile(const std::string& filter, const std::vector<std::string>& extensions)
    {
    	// the s at the end of the string literal tells it to treat \0 as a normal char
    	// we need to do that because of the win32 filter format

		using namespace std::literals::string_literals;

    	uint32 count = extensions.size();
    	std::string win32FilterFormat = filter + "\0"s;

    	for (uint32 index = 0;
    		 index < count;
    		 index++)
    	{
			if (extensions[index][0] == '.')
			{
				win32FilterFormat += "*" + extensions[index];
			}
			else
			{
				win32FilterFormat += "*." + extensions[index];
			}

    		if (index != count - 1)
    		{
    			win32FilterFormat += ";";
    		}
    	}

    	win32FilterFormat += "\0"s;

		OPENFILENAMEA ofn;

		CHAR szFile[1024] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize  = sizeof(OPENFILENAME);
		ofn.hwndOwner    = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().Handle);
		ofn.lpstrFile    = szFile;
		ofn.nMaxFile     = sizeof(szFile);
		ofn.lpstrFilter  = win32FilterFormat.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			std::string path = ofn.lpstrFile;

			std::string cwd = FileSystem::GetCurrentWorkingDirectory();
			std::string result = path.substr(cwd.length() + 1);

			std::replace(result.begin(), result.end(), '\\', '/');
			return result;
		}

		return std::string();
    }

    std::string FileDialog::SaveFile(const std::string& filter, const std::string& extension)
    {
    	// the s at the end of the string literal tells it to treat \0 as a normal char
    	// we need to do that because of the win32 filter format

		using namespace std::literals::string_literals;
		std::string win32FilterFormat;

		if (extension[0] == '.')
		{
			win32FilterFormat = filter + "\0"s + "*"s + extension + "\0"s;
		}
		else
		{
			win32FilterFormat = filter + "\0"s + "*."s + extension + "\0"s;
		}
		
		OPENFILENAMEA ofn;

		CHAR szFile[1024] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize  = sizeof(OPENFILENAME);
		ofn.hwndOwner    = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().Handle);
		ofn.lpstrFile    = szFile;
		ofn.nMaxFile     = sizeof(szFile);
		ofn.lpstrFilter  = win32FilterFormat.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			std::string path = ofn.lpstrFile;
			std::replace(path.begin(), path.end(), '\\', '/');

			// if the extension is not included in the file name add it for convince
			if (path.find_last_of('.') == -1)
			{
				path += extension;
			}

			std::string cwd = FileSystem::GetCurrentWorkingDirectory();
			std::string result = path.substr(cwd.length() + 1);

			return result;
		}

		return std::string();
    }
}

#endif