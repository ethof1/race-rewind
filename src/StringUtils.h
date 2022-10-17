#include "Forward.h"

namespace rrewind
{
	/**
	 * Convenience function to convert a regular std::string to a std::wstring.
	 * 
	 * @warning This function is only safe if @p str contains ASCII characters.
	 *
	 * @param str The normal string to convert
	 * @return The converted string
	 */
	std::wstring convertToWString(const std::string& str);
}

