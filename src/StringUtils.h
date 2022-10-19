#ifndef RREWIND_STRINGUTILS_H
#define RREWIND_STRINGUTILS_H

#include "Forward.h"

namespace rrewind
{
	/**
	 * Convenience function to convert a regular std::string to a std::wstring.
	 * 
	 * @warning This function is safe if @p str only contains ASCII characters.
	 * 
	 * @todo Find a safer conversion method
	 *
	 * @param str The normal string to convert
	 * @return The converted string
	 */
	std::wstring convertToWString(const std::string& str);
}

#endif