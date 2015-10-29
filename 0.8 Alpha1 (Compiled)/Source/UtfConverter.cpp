#include "UtfConverter.h"
#include <locale>
#include <codecvt>

namespace UtfConverter
{
	std::wstring FromUtf8(const std::string& utf8string)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
		return converterX.from_bytes(utf8string);
	}

	std::string ToUtf8(const std::wstring& widestring)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.to_bytes(widestring);
	}
}