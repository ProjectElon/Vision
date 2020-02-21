#include "pch.h"
#include "Vision/Utils/StringUtils.h"

namespace Vision
{
	std::string& StringUtils::RemoveCharsFromLeft(std::string& str, const char* chars)
	{
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	std::string& StringUtils::RemoveCharsFromRight(std::string& str, const char* chars)
	{
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	std::string& StringUtils::Trim(std::string& str, const char* chars)
	{
		return RemoveCharsFromLeft(RemoveCharsFromRight(str, chars), chars);
	}
}