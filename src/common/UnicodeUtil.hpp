/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnicodeUtil.hpp

*******************************************************************************/

#ifndef __UnicodeUtil_HPP
#define __UnicodeUtil_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include "unicode/unistr.h"
#include <string>
//------------------------------------------------------------------------------

namespace Common
{

// Trace an unicode buffer
DEF_Export void UTrace(const char *inMess, const UChar *inStr, int32_t inLength = -1);

//! Convert a standard buffer to an unicode string
void Char2UnicodeString(UnicodeString &outStr, const char *inStr, int32_t inLength = -1);
//! Convert an unicode buffer to string. Cannot be used with accentuated strings !
void UChar2String(std::string &outStr, const UChar *inStr, int32_t inLength = -1);

//! Convert an unicode string to string
DEF_Export std::string UnicodeString2String(const UnicodeString &inStr);

//! Scan an integer in an unicode buffer
int UnicodeStringScanInt(const UChar *inStr);
//! Scan an integer in an unicode string
DEF_Export int UnicodeStringScanInt(const UnicodeString &inStr);
//! Set an integer in an unicode string
void UnicodeStringSetInt(UnicodeString &outStr, int inInt);

//! Scan an unsigned long in an unicode buffer
unsigned long UnicodeStringScanULong(const UChar *inStr);
//! Scan an unsigned long in an unicode string
DEF_Export unsigned long UnicodeStringScanULong(const UnicodeString &inStr);
//! Set an unsigned long in an unicode string
void UnicodeStringSetULong(UnicodeString &outStr, unsigned long inULong);

//! Clear the content between begin and end
/**
	Examples:
	  UnicodeStringStrip(outStr, "a<wd>b</wd>c", "<", ">") => "abc"
		UnicodeStringStrip(outStr, "a<b", "<") => ab
 */
void UnicodeStringStrip(UnicodeString &outStr, const UnicodeString &inStr,
												const UChar *inBegin, const UChar *inEnd = NULL);
//! Get the content between begin and end
bool UnicodeStringGetContent(UnicodeString &outStr, const UnicodeString &inStr,
												const UChar *inBegin, const UChar *inEnd = NULL, unsigned int inNum = 0);
//! Get the nth part of a string
bool UnicodeStringGetLine(UnicodeString &outStr, const UnicodeString &inStr,
												unsigned int inNum = 0, const UChar *inSep = NULL, bool inRemoveCr = false);

//! Add a string with xml special chars replaced with predefined entities
DEF_Export void UnicodeStringAddXmlEntities(UnicodeString &ioStr, const UChar *inStr, int32_t inLength = -1);
//! Add a string with xml special chars replaced with predefined entities
DEF_Export void UnicodeStringAddXmlEntities(UnicodeString &ioStr, const UnicodeString &inStr);

//! Clear the line feeds, trim and remove duplicate spaces in a string
void UnicodeStringClearCrLf(UnicodeString &ioStr);

//! Indicate if a string contains spaces, tabulations or line feeds
bool UnicodeStringHasSpace(UnicodeString &inStr);

//! Tell if an unicode buffer contains only spaces, tabulations or line feeds
bool UnicodeStringIsOnlySpace(const UChar *inStr, int32_t inLength = -1);

//! Write an unicode string to file
bool UnicodeString2File(const UnicodeString &inStr, const char *inFileName);
//! Load a file into an unicode string
DEF_Export bool File2UnicodeString(UnicodeString &outStr, const char *inFileName);

//! UnicodeString buffer to string conversion
DEF_Export bool ConvertUnicodeString2String(std::string &outStr, const UnicodeString &inStr, const char *inConverterName = NULL);
//! UnicodeString to string conversion
DEF_Export bool ConvertUnicodeString2String(std::string &outStr, const UChar *inStr, int32_t inLength, const char *inConverterName = NULL);
//! String to UnicodeString conversion
DEF_Export bool ConvertString2UnicodeString(UnicodeString &outStr, const std::string &inStr, const char *inConverterName = NULL);
//! Char buffer to UnicodeString conversion
DEF_Export bool ConvertString2UnicodeString(UnicodeString &outStr, const char *inStr, int32_t inLength, const char *inConverterName = NULL);

} // namespace Common

//------------------------------------------------------------------------------
#endif
