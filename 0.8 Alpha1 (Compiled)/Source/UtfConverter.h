#pragma once
#include <string>

namespace UtfConverter
{
	std::wstring FromUtf8(const std::string& utf8string);
	std::string ToUtf8(const std::wstring& widestring);
}