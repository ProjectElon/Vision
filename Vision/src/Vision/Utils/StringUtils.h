#pragma once

#include <string>

namespace Vision
{
	class StringUtils
	{
	public:
		static std::string& RemoveCharsFromLeft(std::string& str, const char* chars = "\t\n\v\f\r ");
		static std::string& RemoveCharsFromRight(std::string& str, const char* chars = "\t\n\v\f\r ");
		static std::string& Trim(std::string& str, const char* chars = "\t\n\v\f\r ");

		static bool StartsWith(const std::string& str, const std::string& pattern);
		static bool EndsWith(const std::string& str, const std::string& pattern);
	};
}