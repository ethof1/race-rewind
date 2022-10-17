#include "StringUtils.h"

namespace rrewind
{
	std::wstring convertToWString(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}
}