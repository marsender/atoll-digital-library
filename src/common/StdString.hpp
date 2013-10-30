/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

StdString.hpp

*******************************************************************************/

#ifndef __StdString_HPP
#define __StdString_HPP
//------------------------------------------------------------------------------

#include <string>
//------------------------------------------------------------------------------

namespace Common
{

//! Helper for std::string
class StdString
{
public:
	//! Convert int to std::string
	static std::string Str(int inValue);

	//! Convert unsigned int to std::string
	static std::string Str(unsigned int inValue);

	//! Convert long to std::string
	static std::string Str(long inValue);

	//! Convert unsigned long to std::string
	static std::string Str(unsigned long inValue);

	//! Convert long long to std::string
	static std::string Str(long long inValue);

	//! Convert unsigned long long to std::string
	static std::string Str(unsigned long long inValue);

	//! Convert double to std::string
	static std::string Str(double inValue);

	//! Convert void * to std::string
	static std::string Str(const void * inValue);

	//! Convert char * to std::string
	static inline std::string Str(const char * inValue) { return inValue; }

	//! Convert bool to std::string
	static inline std::string Str(bool inValue) { return std::string(1, '0' | static_cast<char>(inValue)); }

	//! Appends a boolean value as 'true' or 'false' to the end of a string
	static std::string &AppendBool(std::string &ioStr, bool inBool);

	//! Appends an integer to the end of a string
	static std::string &AppendInt(std::string &ioStr, int inInt);

	//! Appends an unsigned integer to the end of a string
	static std::string &AppendUInt(std::string &ioStr, unsigned int inInt);
	
	//! Lowercase a string
	static std::string &ToLower(std::string &ioStr);
	
	//! Replace all occurrences of characters in find with the characters in repl
	static std::string &FindAndReplace(std::string &ioStr, const char *inFind, const char *inRepl);
	//! Replace all occurrences of characters in find with the characters in repl
	static std::string &FindAndReplace(std::string &ioStr, const std::string &inFind, const std::string &inRepl);

	//! Get the current date time, with the format: yyyy/mm/jj hh:mm:ss
	static std::string TimeToString();
};

} // namespace Common

//------------------------------------------------------------------------------
#endif
