/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnicodeUtil.cpp

*******************************************************************************/

#include "UnicodeUtil.hpp"
#include "AppAssert.hpp" // in common directory
#include "Logger.hpp" // in common directory
#include "Trace.hpp" // in common directory
#include "unicode/ucnv.h"
#include "unicode/ustring.h" // u_strstr, u_strchr...
#include "unicode/ustdio.h" // u_sprintf
#ifndef WIN32
	#include <string.h>
#endif
//------------------------------------------------------------------------------

// Want UString fonction for utf8 <=> char conversion ?
const bool cWantUString = true;
static const UChar cSpaceChar[] = { ' ', '\t', '\r', '\n', 0 };
//------------------------------------------------------------------------------

using namespace Common;

void Common::UTrace(const char *inMess, const UChar *inStr, int32_t inLength /*= -1*/)
{
	std::string info;
	if (inStr != NULL)
		ConvertUnicodeString2String(info, inStr, inLength);

	// Trace via logger or in trace file
	bool wantLogger = false;
	if (wantLogger) {
		bool bakOs1 = gLog.mEnableOs1;
		gLog.mEnableOs1 = false; // Disable first output stream (console log)
		// Set debug log level
		bool isDebug = gLog.isInLogLevel(eTypLogDebug);
		gLog.addLogLevel(eTypLogDebug); // Show debug
		// Log
		if (inMess == NULL)
			gLog.log(eTypLogDebug, "Deb > %s", info.c_str());
		else
			gLog.log(eTypLogDebug, "Deb > %s --- %s", inMess, info.c_str());
		// Restore debug log level
		if (!isDebug)
			gLog.removeLogLevel(eTypLogDebug); // Remove debug
		gLog.mEnableOs1 = bakOs1; // Restore first output stream (console log)
	}
	else {
		if (inMess == NULL)
			Trace("%s", info.c_str());
		else
			Trace("%s --- %s", inMess, info.c_str());
	}
}
//------------------------------------------------------------------------------

void Common::Char2UnicodeString(UnicodeString &outStr, const char *inStr, int32_t inLength)
{
	int32_t len = inLength == -1 ? strlen(inStr) : inLength;

	// Test string content
	for (int32_t i = 0; i < len; i++) {
		if ((unsigned char)inStr[i] > 0x007F) {
			ConvertString2UnicodeString(outStr, inStr, inLength);
			//gLog.log(eTypLogError, "Err > Char2UnicodeString need converter: '%s'", inStr);
			return;
		}
	}

	UChar *buf = new UChar[len + 1];
	u_uastrncpy(buf, inStr, len);
	buf[len] = 0;
	outStr = buf;
	delete [] buf;
}
//------------------------------------------------------------------------------

void Common::UChar2String(std::string &outStr, const UChar *inStr, int32_t inLength /*= -1*/)
{
	int32_t len = inLength == -1 ? u_strlen(inStr) : inLength;

	// Test string content
	for (int32_t i = 0; i < len; i++) {
		if (inStr[i] > 0x007F) {
			ConvertUnicodeString2String(outStr, inStr, inLength);
			//gLog.log(eTypLogError, "Err > UChar2String need converter: '%s'", outStr.c_str());
			return;
		}
	}

	// Copy string
	char *buf = new char[len + 1];
	u_austrncpy(buf, inStr, len);
	buf[len] = 0;
	outStr = buf;
	delete [] buf;
}
//------------------------------------------------------------------------------

std::string Common::UnicodeString2String(const UnicodeString &inStr)
{
	const UChar *buf = inStr.getBuffer();
	int32_t length = inStr.length();

	if (length == 0)
		return "";

	std::string str;
	UChar2String(str, buf, length);

	return str;
}
//------------------------------------------------------------------------------

int Common::UnicodeStringScanInt(const UChar *inStr)
{
	int32_t i = 0;
	if (u_sscanf(inStr, "%d", &i) != 1) {
		std::string str;
		UChar2String(str, inStr);
		gLog.log(eTypLogError, "Err > Unicode sscanf 'int' error on string: '%s'", str.c_str());
	}

	return i;
}
//------------------------------------------------------------------------------

int Common::UnicodeStringScanInt(const UnicodeString &inStr)
{
	UnicodeString s(inStr);

	return UnicodeStringScanInt(s.getTerminatedBuffer());
}
//------------------------------------------------------------------------------

void Common::UnicodeStringSetInt(UnicodeString &outStr, int inInt)
{
	UChar buf[50];
	int32_t len = u_sprintf(buf, "%d", inInt);
	if (len == 0) {
		*buf = 0;
		gLog.log(eTypLogError, "Err > Unicode sprintf 'int' error");
	}

	outStr.setTo(buf, len);
}
//------------------------------------------------------------------------------

unsigned long Common::UnicodeStringScanULong(const UChar *inStr)
{
	unsigned long i = 0;
	if (u_sscanf(inStr, "%lu", &i) != 1) {
		std::string str;
		UChar2String(str, inStr);
		gLog.log(eTypLogError, "Err > Unicode sscanf 'ulong' error on string: '%s'", str.c_str());
	}

	return i;
}
//------------------------------------------------------------------------------

unsigned long Common::UnicodeStringScanULong(const UnicodeString &inStr)
{
	UnicodeString s(inStr);

	return UnicodeStringScanULong(s.getTerminatedBuffer());
}
//------------------------------------------------------------------------------

void Common::UnicodeStringSetULong(UnicodeString &outStr, unsigned long inULong)
{
	UChar buf[50];
	int32_t len = u_sprintf(buf, "%lu", inULong);
	if (len == 0) {
		*buf = 0;
		gLog.log(eTypLogError, "Err > Unicode sprintf 'ulong' error");
	}

	outStr.setTo(buf, len);
}
//------------------------------------------------------------------------------

void Common::UnicodeStringStrip(UnicodeString &outStr, const UnicodeString &inStr,
																const UChar *inBegin, const UChar *inEnd /*= NULL*/)
{
	int32_t len = inStr.length();
	if (len == 0) {
		outStr.remove();
		return;
	}

	if (!inBegin || *inBegin == 0) {
		gLog.log(eTypLogError, "Err > Unicode string strip incorrect begin param");
		outStr = inStr;
    return;
	}

	// Capacity of input string may be egal to length
	// If it is the case, we must use a new buffer
	UChar *s;
	UnicodeString *strCopy = NULL;
	int32_t cap = inStr.getCapacity();
	if (len < cap) {
		s = const_cast<UChar *>(inStr.getBuffer());
		s[len] = 0;
	}
	else {
		strCopy = new UnicodeString(inStr);
		s = const_cast<UChar *>(strCopy->getTerminatedBuffer());
	}

	UChar *d, *p;
  int32_t lDeb = u_strlen(inBegin);
	int32_t lFin = inEnd ? u_strlen(inEnd) : 0;
	p = u_strstr(s, inBegin);
  d = new UChar[len + 1];

  // Copy s in d, without the content between begin and end
  *d = 0;
  while (true) {
    *p = 0;
    u_strcat(d, s);
    *p = *(inBegin);
    // Go after the begining string
    s = p + lDeb;
    // Go after the ending string, if not null
    if (inEnd) {
      s = u_strstr(s, inEnd);
      if (s)
        s += lFin;
      else
        break; //s = u_strchr(s, 0);
    }
    if ((p = u_strstr(s, inBegin)) == NULL) {
      u_strcat(d, s);
      break;
    }
  }

  outStr = d;
  delete [] d;
	if (strCopy)
		delete strCopy;
}
//------------------------------------------------------------------------------

bool Common::UnicodeStringGetContent(UnicodeString &outStr, const UnicodeString &inStr,
																const UChar *inBegin, const UChar *inEnd /*= NULL*/,
																unsigned int inNum /*= 0*/)
{
	int32_t len = inStr.length();
	if (len == 0) {
		outStr.remove();
		return false;
	}

	if (!inBegin || *inBegin == 0) {
		gLog.log(eTypLogError, "Err > Unicode string get content incorrect begin param");
		outStr = inStr;
    return false;
	}

	// Capacity of input string may be egal to length
	// If it is the case, we must use a new buffer
	UChar *s;
	UnicodeString *strCopy = NULL;
	int32_t cap = inStr.getCapacity();
	if (len < cap) {
		s = const_cast<UChar *>(inStr.getBuffer());
		s[len] = 0;
	}
	else {
		strCopy = new UnicodeString(inStr);
		s = const_cast<UChar *>(strCopy->getTerminatedBuffer());
	}

	// Go to the right content number
	UChar *p, *p2;
	unsigned int numOcc = 0;
  int32_t lDeb = u_strlen(inBegin);
	p = s;
  while ((p = u_strstr(p, inBegin)) != NULL) {
    p += lDeb;
    if (numOcc++ == inNum)
      break;
  }
  if (!p) {
		outStr.remove();
    return false;
	}

	// No end patern ?
	if (!inEnd || *inEnd == 0) {
		outStr = p;
		return true;
	}

	// No end ?
	if ((p2 = u_strstr(p, inEnd)) == NULL) {
		outStr = p;
		return false;
	}

	// Get the content
	*p2 = 0;
	outStr = p;
	*p2 = *(inEnd);

	if (strCopy)
		delete strCopy;

	return true;
}
//------------------------------------------------------------------------------

bool Common::UnicodeStringGetLine(UnicodeString &outStr, const UnicodeString &inStr,
	unsigned int inNum /*= 0*/, const UChar *inSep /*= NULL*/, bool inRemoveCr /*= false*/)
{
	int32_t len = inStr.length();
	if (len == 0) {
		outStr.remove();
		return false;
	}

	if (inSep && *inSep == 0) {
		gLog.log(eTypLogError, "Err > Unicode string get line incorrect sep param");
		outStr = inStr;
    return false;
	}

	const UChar *sep;
	UChar defaultSep[] = { '\n', 0 };
	sep = inSep ? inSep : defaultSep;

	// Capacity of input string may be egal to length
	// If it is the case, we must use a new buffer
	UChar *s;
	UnicodeString *strCopy = NULL;
	int32_t cap = inStr.getCapacity();
	if (len < cap) {
		s = const_cast<UChar *>(inStr.getBuffer());
		s[len] = 0;
	}
	else {
		strCopy = new UnicodeString(inStr);
		s = const_cast<UChar *>(strCopy->getTerminatedBuffer());
	}

	// Go to the right content number
	UChar *p, *p2;
	unsigned int numOcc = 0;
  int32_t lDeb = u_strlen(sep);
	p = s;
	while (numOcc == 0 || (p = u_strstr(p, sep)) != NULL) {
		p += (numOcc == 0) ? 0 : lDeb;
		if (numOcc++ == inNum)
			break;
	}
	if (!p) {
		outStr.remove();
		return false;
	}

	// Search the end
	p2 = u_strstr(p, sep);
	if (p2 == NULL) {
		p2 = u_strchr(p, 0);
	}
	if (inRemoveCr) {
		if (p2 > p && *(p2 - 1) == '\r')
			p2--;
	}

	// Get the content
	*p2 = 0;
	outStr = p;
	*p2 = *(sep);

	if (strCopy)
		delete strCopy;

	return true;
}
//------------------------------------------------------------------------------

void Common::UnicodeStringAddXmlEntities(UnicodeString &ioStr, const UChar *inStr, int32_t inLength /*= -1*/)
{
	const UChar xmlChar[] = { '&', '<', '>', '"', '\'' };

	if (inLength == -1) {
		if (u_strpbrk(inStr, xmlChar) == NULL) {
			ioStr += inStr;
			return;
		}
	}
	else {
		bool isOneXmlChar = false;
		for (int32_t i = 0; i < inLength; ++i) {
			if (u_strchr(xmlChar, inStr[i])) {
				isOneXmlChar = true;
				break;
			}
		}
		if (!isOneXmlChar) {
			ioStr.append(inStr, inLength);
			return;
		}
	}

	while (*inStr) {
		switch (*inStr) {
		case '&':
			ioStr += "&amp;";
			break;
		case '<':
			ioStr += "&lt;";
			break;
		case '>':
			ioStr += "&gt;";
			break;
		case '"':
			ioStr += "&quot;";
			break;
		case '\'':
			ioStr += "&apos;";
			break;
		default:
			ioStr += *inStr;
			break;
		}
		if (inLength != -1) {
			if (--inLength == 0)
				break;
		}
		inStr++;
	}
}
//------------------------------------------------------------------------------

void Common::UnicodeStringAddXmlEntities(UnicodeString &ioStr, const UnicodeString &inStr)
{
	UnicodeString &str = (UnicodeString &)inStr; // Const hack
	UnicodeStringAddXmlEntities(ioStr, str.getTerminatedBuffer());
}
//------------------------------------------------------------------------------

void Common::UnicodeStringClearCrLf(UnicodeString &ioStr)
{
	int32_t len, newLen;
	UChar *ptr, *buf;

	// Trim left and remove duplicate space chars
	bool isSpace = true;
	newLen = len = ioStr.length();
	buf = ptr = const_cast<UChar *>(ioStr.getBuffer());
	for (int32_t i = 0; i < len; ++i) {
		switch (*ptr) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			if (isSpace)
				newLen--;
			else {
				*buf++ = ' ';
			}
			isSpace = true;
			ptr++;
			break;
		default:
			isSpace = false;
			*buf++ = *ptr++;
			break;
		}
	}

	// Trim right
	while (newLen) {
		switch (*(buf - 1)) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			buf--;
			newLen--;
			continue;
		default:
			break;
		}
		break;
	}

	// Adjust the string length if it has changed
	if (newLen < len) {
		ioStr.truncate(newLen);
		*buf = 0; // Just for readability
	}
}
//------------------------------------------------------------------------------

bool Common::UnicodeStringHasSpace(UnicodeString &inStr)
{
	int32_t len = inStr.length();
	const UChar *buf = inStr.getBuffer();

	for (int32_t i = 0; i < len; ++i) {
		if (u_strchr(cSpaceChar, buf[i]))
			return true;
	}

	return false;
}
//------------------------------------------------------------------------------

bool Common::UnicodeStringIsOnlySpace(const UChar *inStr, int32_t inLength)
{
	int32_t len = inLength == -1 ? u_strlen(inStr) : inLength;

	for (int32_t i = 0; i < len; ++i) {
		if (!u_strchr(cSpaceChar, inStr[i]))
			return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool Common::UnicodeString2File(const UnicodeString &inStr, const char *inFileName)
{
	UFILE *f = NULL;
	if ((f = u_fopen(inFileName, "w", NULL, "utf-8")) == NULL) {
		gLog.log(eTypLogError, "Err > Open file for writing error: %s", inFileName);
		return false;
	}

	int32_t length;
	length = inStr.length();
	u_file_write(inStr.getBuffer(), length, f);
	// It's normal that write size is different from length

	u_fclose(f);

  return true;
}
//------------------------------------------------------------------------------

bool Common::File2UnicodeString(UnicodeString &outStr, const char *inFileName)
{
  AppAssert(inFileName != NULL);

	outStr.remove();

	UFILE *f = NULL;
	if ((f = u_fopen(inFileName, "r", NULL, "utf-8")) == NULL) {
		gLog.log(eTypLogError, "Err > Open file for reading error: %s", inFileName);
		return false;
	}

  UChar *buffer;
	int32_t read, size, total;

	total = 0;
	size = 0xFFFu;	// 4095
	buffer = new UChar[size];
	while (true) {
		read = u_file_read(buffer, size, f);
    if (!read)
      break;
		total += read;
		outStr.append(buffer, read);
	}
	delete [] buffer;

	/* Read char by char
	UChar32 c;
	while ((c = u_fgetcx(f)) != U_EOF) {
		outStr += c;
	}
	*/

	u_fclose(f);

  return true;
}
//------------------------------------------------------------------------------

bool Common::ConvertUnicodeString2String(std::string &outStr, const UnicodeString &inStr,
																				 const char *inConverterName /*= NULL*/)
{
	bool isOk = ConvertUnicodeString2String(outStr, inStr.getBuffer(), inStr.length(), inConverterName);

	return isOk;
}
//------------------------------------------------------------------------------

bool Common::ConvertUnicodeString2String(std::string &outStr, const UChar *inStr, int32_t inLength,
																				 const char *inConverterName)
{
	bool isOk = true;
	int32_t len;
	UConverter *conv;
	UErrorCode status = U_ZERO_ERROR;
	char *target = NULL;

	int32_t inputLength = inLength == -1 ? u_strlen(inStr) : inLength;

	if (inputLength == 0) {
		outStr.clear();
		return true;
	}

	if (cWantUString && inConverterName == NULL) {
		// Create the target buffer
		int32_t targetLength = UCNV_GET_MAX_BYTES_FOR_STRING(inputLength, 3);
		target = new char[targetLength];
		// Doc: http://www.icu-project.org/apiref/icutargetLength4c/ustring_8h.html
		u_strToUTF8(target, targetLength, &len, inStr, inputLength, &status);
		// Store the target if no error
		if (U_SUCCESS(status))
			outStr.assign(target, len);
		else {
			outStr.clear();
			gLog.log(eTypLogError, "Err > ConvertUnicodeString2String with UString: %s", u_errorName(status));
			isOk = false;
		}
		delete [] target;
		return isOk;
	}

	// Test string content
	for (int32_t i = 0; i < inputLength; i++) {
		if (inStr[i] > 0x007F) {
			isOk = false;
			break;
		}
	}
	if (isOk) {
		// Copy string
		char *buf = new char[inputLength + 1];
		u_austrncpy(buf, inStr, inputLength);
		buf[inputLength] = 0;
		outStr = buf;
		delete [] buf;
		return true;
	}

	isOk = true;
	outStr.clear();

	// Set up the converter
	// Valid converters: "SCSU" "utf-8"
	conv = ucnv_open(inConverterName ? inConverterName : "utf-8", &status);
	if (!U_SUCCESS(status)) {
		gLog.log(eTypLogError, "Err > ConvertUnicodeString2String open converter: %s", u_errorName(status));
		return false;
	}

	// Create the target buffer
	int32_t targetLength = UCNV_GET_MAX_BYTES_FOR_STRING(inputLength, ucnv_getMaxCharSize(conv));
	target = new char[targetLength];

	// Convert the string
	len = ucnv_fromUChars(conv, target, targetLength, inStr, inputLength, &status);
	if (!U_SUCCESS(status)) {
		gLog.log(eTypLogError, "Err > ConvertUnicodeString2String fromUChars conversion: %s", u_errorName(status));
		isOk = false;
	}

	// Store the target
	outStr.assign(target, len);

	delete [] target;

	// Close the converter
	ucnv_close(conv);

	return isOk;
}
//------------------------------------------------------------------------------

bool Common::ConvertString2UnicodeString(UnicodeString &outStr, const std::string &inStr,
																				 const char *inConverterName /*= NULL*/)
{
	return ConvertString2UnicodeString(outStr, inStr.c_str(), (int32_t)inStr.length(), inConverterName);
}
//------------------------------------------------------------------------------

bool Common::ConvertString2UnicodeString(UnicodeString &outStr, const char *inStr, int32_t inLength,
																				 const char *inConverterName /*= NULL*/)
{
	bool isOk = true;
	int32_t len;
	UConverter *conv;
	UErrorCode status = U_ZERO_ERROR;
	UChar *uTarget = NULL;

	int32_t inputLength = inLength == -1 ? strlen(inStr) : inLength;

	if (inputLength == 0) {
		outStr.remove();
		return true;
	}

	if (cWantUString && inConverterName == NULL) {
		// Create the target buffer
		int32_t targetLength = 2 * inputLength;
		uTarget = new UChar[targetLength];
		// Doc: http://www.icu-project.org/apiref/icu4c/ustring_8h.html
		u_strFromUTF8(uTarget, targetLength, &len, inStr, inputLength, &status);
		// Store the target if no error
		if (U_SUCCESS(status))
			outStr.setTo(uTarget, len);
		else {
			outStr.remove();
			gLog.log(eTypLogError, "Err > ConvertString2UnicodeString with UString: %s", u_errorName(status));
			isOk = false;
		}
		delete [] uTarget;
		return isOk;
	}

	// Test string content
	for (int32_t i = 0; i < inputLength; i++) {
		if ((unsigned char)inStr[i] > 0x007F) {
			isOk = false;
			break;
		}
	}
	if (isOk) {
		// Copy string
		UChar *buf = new UChar[inputLength + 1];
		u_uastrncpy(buf, inStr, inputLength);
		buf[inputLength] = 0;
		outStr = buf;
		delete [] buf;
		return true;
	}

	isOk = true;
	outStr.remove();

	// Set up the converter
	// Valid converters: "SCSU" "utf-8"
	status = U_ZERO_ERROR;
	conv = ucnv_open(inConverterName ? inConverterName : "utf-8", &status);
	if (!U_SUCCESS(status)) {
		gLog.log(eTypLogError, "Err > ConvertString2UnicodeString open converter: %s", u_errorName(status));
		return false;
	}

	// Create the target buffer
	int32_t targetLength = 2 * inputLength;
	uTarget = new UChar[targetLength];

	// Convert the string
	len = ucnv_toUChars(conv, uTarget, targetLength, inStr, inputLength, &status);
	if (!U_SUCCESS(status)) {
		gLog.log(eTypLogError, "Err > ConvertString2UnicodeString toUChars conversion: %s", u_errorName(status));
		isOk = false;
	}

	// Store the target
	outStr.setTo(uTarget, len);

	delete [] uTarget;

	// Close the converter
	ucnv_close(conv);

	return isOk;
}
//------------------------------------------------------------------------------
