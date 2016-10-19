/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Containers.cpp

*******************************************************************************/

#include "Containers.hpp" // In common directory
#include "AppAssert.hpp" // In common directory
#include "Logger.hpp" // In common directory
#include "UnicodeUtil.hpp" // In common directory
#include "unicode/ustring.h"
#include "unicode/coll.h"
//------------------------------------------------------------------------------

/**
 Unicode Character reference
 Eg for 2014: http://www.fileformat.info/info/unicode/char/2014/index.htm
 0x0060 GRAVE ACCENT
 0x00A0 NO-BREAK SPACE
 0x202F NARROW NO-BREAK SPACE
 0x00A9 COPYRIGHT SIGN
 0x00AB LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
 0x00B4 ACUTE ACCENT
 0x00BB RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
 0x2014 EM DASH
 0x2018 LEFT SINGLE QUOTATION MARK
 0x2019 RIGHT SINGLE QUOTATION MARK
 0x201C LEFT DOUBLE QUOTATION MARK
 0x201D RIGHT DOUBLE QUOTATION MARK
 0x0301 COMBINING ACUTE ACCENT
*/
static const UChar cSpaceChar[] = { ' ', '\t', '\r', '\n', 0 };
static const UChar cSepChar[] = { '\'', 0x00AD, 0x2019, 0x2014, '-', '.', ',', ';', ':', '!', '?', '*', '/', 0 };
static const UChar cFioChar[] = { 0x00A0, 0x202F, '"', 0x00AB, 0x00BB, 0x2018, 0x00B4, 0x0060, 0x201C, 0x201D, 0x0301,
	'<', '>', '+', '=', 0x00A9, '%', '~', '|', '#', '\\', '{', '}', '[', ']', '(', ')', 0 }; // '¶' '' '§'
//------------------------------------------------------------------------------

using namespace Common;

Collator *gCollator = NULL; //!< Global collator instantiation that can be used by all classes
//------------------------------------------------------------------------------

// UnicodeString Compare function
bool fCmpUnicodeString::operator() (const UnicodeString &s1, const UnicodeString &s2) const
{
	// Speed up simple char comparisson
	/*{
		int lexiCmp;
		bool isUnicode = false;
		size_t l1 = s1.length();
		size_t l2 = s1.length();
		size_t len = l1 > l2 ? l2 : l1;
		const UChar *p1 = s1.getBuffer();
		const UChar *p2 = s2.getBuffer();
		for (; len--; ++p1, ++p2) {
			if (*p1 > 127 || *p2 > 127) {
				isUnicode = true;
				break;
			}
			lexiCmp = (int)*p1 - (int)*p2;
			if (lexiCmp != 0)
				return (lexiCmp < 0);
		}
		//if (!isUnicode) {
			lexiCmp = ((long)l1 - (long)l2);
			return (lexiCmp < 0);
		//}
	}*/

	bool wantDebug = false;
	if (wantDebug) {
		UnicodeString uBuf(s1);
		uBuf += " <=> ";
		uBuf += s2;
		std::string buf = UnicodeString2String(uBuf);
		gLog.log(eTypLogDebug, "Deb > Collator compare: %s", buf.c_str());
	}

	if (gCollator == NULL) {
		gLog.log(eTypLogError, "Err > fCmpUnicodeString missing collator");
		int8_t cmp = s1.compare(s2);
		return (cmp == -1);
	}

	// 0 if string a == string b
	// 1 if string a > string b
	// -1 if string a < string b
	UErrorCode status = U_ZERO_ERROR;
	UCollationResult cmp = gCollator->compare(s1, s2, status);
	if (!U_SUCCESS(status)) {
		UnicodeString uBuf(s1);
		uBuf += " <=> ";
		uBuf += s2;
		std::string buf = UnicodeString2String(uBuf);
		gLog.log(eTypLogError, "Err > Collator compare error %d: %s", status, buf.c_str());
	}

	if (wantDebug)
		gLog.log(eTypLogDebug, "Deb > Collator compare result: %d", cmp);

	return (cmp == UCOL_LESS);
}
//------------------------------------------------------------------------------

int32_t Common::CollatorStrcmp(const UChar *s1, const UChar *s2)
{
	if (gCollator == NULL) {
		gLog.log(eTypLogError, "Err > Common::CollatorStrcmp missing collator");
		return -1;
	}

	// Direct use of the collator function
	int32_t l1 = u_strlen(s1);
	int32_t l2 = u_strlen(s2);

	Collator::ECollationStrength strength = gCollator->getStrength();
	gCollator->setStrength(Collator::PRIMARY);

	// 0 if string a == string b
	// 1 if string a > string b
	// -1 if string a < string b
	UErrorCode status = U_ZERO_ERROR;
	UCollationResult cmp = gCollator->compare(s1, l1, s2, l2, status);
	if (!U_SUCCESS(status)) {
		UnicodeString uBuf(s1, l1);
		uBuf += " <=> ";
		uBuf.append(s2, l2);
		std::string buf = UnicodeString2String(uBuf);
		gLog.log(eTypLogError, "Err > CollatorStrcmp compare error %d: %s", status, buf.c_str());
	}

	gCollator->setStrength(strength);

	return cmp;
}
//------------------------------------------------------------------------------

int32_t Common::CollatorStrncmp(const UChar *s1, const UChar *s2, int32_t n)
{
	if (gCollator == NULL) {
		gLog.log(eTypLogError, "Err > Common::CollatorStrncmp missing collator");
		return -1;
	}

	// Direct use of the collator function
	int32_t l1 = u_strlen(s1);
	if (l1 > n)
		l1 = n;
	int32_t l2 = u_strlen(s2);
	if (l2 > n)
		l2 = n;

	Collator::ECollationStrength strength = gCollator->getStrength();
	gCollator->setStrength(Collator::PRIMARY);

	// 0 if string a == string b
	// 1 if string a > string b
	// -1 if string a < string b
	UErrorCode status = U_ZERO_ERROR;
	UCollationResult cmp = gCollator->compare(s1, l1, s2, l2, status);
	if (!U_SUCCESS(status)) {
		UnicodeString uBuf(s1, l1);
		uBuf += " <=> ";
		uBuf.append(s2, l2);
		std::string buf = UnicodeString2String(uBuf);
		gLog.log(eTypLogError, "Err > CollatorStrncmp compare error %d: %s", status, buf.c_str());
	}

	gCollator->setStrength(strength);

	return cmp;
}
//------------------------------------------------------------------------------

size_t Common::UnicodeStringGetWords(UnicodeStringSet &outStringSet, UnicodeString &inStr)
{
	// This code is not used
	// See AdornerDecorator::DecorateContent for word split

	UChar *s = const_cast<UChar *>(inStr.getTerminatedBuffer());

	outStringSet.clear();

	// Split the content in words, separated by spaces or line feed
	// Warning: the initial string is destroyed
	UnicodeString word;
	UChar *ptr, *state;
	ptr = u_strtok_r(s, cSpaceChar, &state);
	while (ptr != NULL) {
		// Add the word in the word list
		word = ptr;
		outStringSet.insert(word);
		// Get next word
		ptr = u_strtok_r(NULL, cSpaceChar, &state);
	}

	// Clears the string, as it is bogus now
	inStr.remove();

	return outStringSet.size();
}
//------------------------------------------------------------------------------

size_t Common::UnicodeStringGetSubWords(UnicodeStringSet &outStringSet, UnicodeString &inStr)
{
	UChar *s = const_cast<UChar *>(inStr.getTerminatedBuffer());

	outStringSet.clear();

	// Cut the word in parts, separated by '-' or '.' etc...
	UnicodeString word;
	UChar *ptr, *state;
	UChar *ptr2, *state2;
	ptr = u_strtok_r(s, cSepChar, &state);
	while (ptr != NULL) {
		// Clear each part from it's punctuation, parenthesis etc...
		ptr2 = u_strtok_r(ptr, cFioChar, &state2);
		while (ptr2 != NULL) {
			word = ptr2;
			outStringSet.insert(word);
			ptr2 = u_strtok_r(NULL, cFioChar, &state2);
		}
		// Get next token
		ptr = u_strtok_r(NULL, cSepChar, &state);
	}

	// Clears the string, as it is bogus now
	inStr.remove();

	return outStringSet.size();
}
//------------------------------------------------------------------------------
