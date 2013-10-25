/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "StringBuffer.hpp"
#include <math.h> // For pow and log10
#include "unicode/unistr.h"
#include "unicode/ustring.h"
//#include "Misc.h"

// Formerly, StringBuffer used 1024 as the default size of its internal buffer.
// However, StringBuffer is used primarily for token- and term-oriented
// processing, e.g. in StandardTokenizer.  I've calculated that the average
// token (as produced by StandardTokenizer) in all .txt files distributed in
// the Project Gutenberg CD Image (August 2003 release) has only 6 characters.
// Although most languages are likely to have a longer average word length than
// English due to the popularity of "non-atomized" conjugation and declension
// mechanisms, 1024 is still vastly excessive.
// I made two changes intended to deliver better overall performance:
//   a) Switched to a default StringBuffer character capacity of 32.  Though 32
//      is longer than the average token, the high cost of realloc makes a
//      slightly liberal default size optimal.  I chose the default size of 32
//      after fairly extensive experimentation on the Gutenberg e-texts.  The
//      results are summarized in the following table:
//      ------------------------------------------------------------------------
//      LUCENE_DEFAULT_TOKEN_BUFFER_SIZE value | % faster than default size 1024
//      ------------------------------------------------------------------------
//                                           8 : 4%
//                                          16 : 7%
//                                          32 : 6%
//                                          64 : 3%
//      A default size of 32 is actually slightly slower than 16, but I was
//      experimenting on English text; I expect that 32 will maintain decent
//      performance in languages such as German, and in technical documents
//      with long tokens.
//
//   b) To offset the switch to a smaller default buffer size, I implemented a
//      more aggressive growth strategy.  A StringBuffer now [at least] doubles
//      the size of its internal buffer every time it needs to grow, rather
//      than [at least] increasing by LUCENE_DEFAULT_TOKEN_BUFFER_SIZE no
//      matter how many times it has already grown.
#define LUCENE_DEFAULT_TOKEN_BUFFER_SIZE 32

U_STRING_DECL(cStrDot, ".", 1);
U_STRING_DECL(cStrZero, "0", 1);
CL_NS_DEF(util)
void StringBuffer::InitStringDecl() {
	U_STRING_INIT(cStrDot, ".", 1);
	U_STRING_INIT(cStrZero, "0", 1);
}
CL_NS_END

using namespace Common;

UChar* lucene_i64tot(
  int64_t value, /* [I] Value to be converted */
  UChar* str,      /* [O] Destination for the converted value */
  int radix)      /* [I] Number base for conversion */
{
	uint64_t val;
	int negative;
	UChar buffer[65];
	UChar* pos;
	int digit;

	if (value < 0 && radix == 10) {
		negative = 1;
		val = -value;
	}
	else {
		negative = 0;
		val = value;
	} /* if */

	pos = &buffer[64];
	*pos = '\0';

	do {
		digit = val % radix;
		val = val / radix;
		if (digit < 10) {
			*--pos = '0' + digit;
		}
		else {
			*--pos = 'a' + digit - 10;
		} /* if */
	}
	while (val != 0L);

	if (negative) {
		*--pos = '-';
	} /* if */

	_tcsncpy(str, pos, &buffer[64] - pos + 1); // Needed for unicode to work
	return str;
}

int64_t lucene_tcstoi64(const UChar* str, UChar**end, int radix)
{
#define LUCENE_TCSTOI64_RADIX(x,r) ((x>=_T('0') && x<=_T('9'))?x-_T('0'):((x>=_T('a') && x<=_T('z'))?x-_T('a')+10:((x>=_T('A') && x<=_T('Z'))?x-_T('A')+10:1000)))

	if (radix < 2 || radix > 36)
		return 0;

	/* Skip white space.  */
	while (_istspace (*str))
		++str;

	int sign = 1;
	if (str[0] == _T('+'))
		str++;
	else if (str[0] == _T('-')) {
		sign = -1;
		str++;
	}

	*end = (UChar*)str;
	long r = -1;
	while ((r = LUCENE_TCSTOI64_RADIX(*end[0], radix)) >= 0 && r < radix)
		(*end)++;

	UChar* p = (*end) - 1;
	int64_t ret = 0;
	int pos = 0;
	for (;p >= str;p--) {
		int i = LUCENE_TCSTOI64_RADIX(p[0], radix);
		if (pos == 0)
			ret = i;
		else
			ret += (int64_t)pow((float_t)radix, (float_t)pos) * i; //todo: might be quicker with a different pow overload

		pos++;
	}
	return sign*ret;
}

CL_NS_DEF(util)

StringBuffer::StringBuffer(UChar *buf, int32_t maxlen, const bool consumeBuffer)
{
	buffer = buf;
	bufferLength = maxlen;
	bufferOwner = !consumeBuffer;
	len          = 0;
}

StringBuffer::StringBuffer()
{
	//Func - Constructor. Allocates a buffer with the default length.
	//Pre  - true
	//Post - buffer of length bufferLength has been allocated

	//Initialize
	bufferLength = LUCENE_DEFAULT_TOKEN_BUFFER_SIZE;
	len          = 0;
	//Allocate a buffer of length bufferLength
	buffer       = _CL_NEWARRAY(UChar, bufferLength);
	bufferOwner  = true;
}

StringBuffer::StringBuffer(const int32_t initSize)
{
	//Func - Constructor. Allocates a buffer of length initSize + 1
	//Pre  - initSize > 0
	//Post - A buffer has been allocated of length initSize + 1

	//Initialize the bufferLength to initSize + 1 The +1 is for the terminator '\0'
	bufferLength = initSize + 1;
	len = 0;
	//Allocate a buffer of length bufferLength
	buffer = _CL_NEWARRAY(UChar, bufferLength);
	bufferOwner  = true;
}

StringBuffer::StringBuffer(const UChar *value)
{
	//Func - Constructor.
	//       Creates an instance of Stringbuffer containing a copy of the string value
	//Pre  - value != NULL
	//Post - An instance of StringBuffer has been created containing the copy of the string value

	//Initialize the length of the string to be stored in buffer
	len = (int32_t) _tcslen(value);

	//Calculate the space occupied in buffer by a copy of value
	const int32_t occupiedLength = len + 1;

	// Minimum allocated buffer length is LUCENE_DEFAULT_TOKEN_BUFFER_SIZE.
	bufferLength = (occupiedLength >= LUCENE_DEFAULT_TOKEN_BUFFER_SIZE
	                ? occupiedLength : LUCENE_DEFAULT_TOKEN_BUFFER_SIZE);

	//Allocate a buffer of length bufferLength
	buffer = _CL_NEWARRAY(UChar, bufferLength);
	bufferOwner  = true;
	//Copy the string value into buffer
	_tcsncpy(buffer, value, occupiedLength);
	//Assert that the buffer has been terminated at the end of the string
	CND_PRECONDITION (buffer[len] == '\0', "Buffer was not correctly terminated");
}

StringBuffer::~StringBuffer()
{
	// Func - Destructor
	// Pre  - true
	// Post - Instanc has been destroyed

	if (bufferOwner) {
		_CLDELETE_CARRAY(buffer);
	}
	else
		buffer = NULL;
}

void StringBuffer::clear()
{
	//Func - Clears the Stringbuffer and resets it to it default empty state
	//Pre  - true
	//Post - pre(buffer) has been destroyed and a new one has been allocated

	//Destroy the current buffer if present
	_CLDELETE_CARRAY(buffer);

	//Initialize
	len = 0;
	bufferLength = LUCENE_DEFAULT_TOKEN_BUFFER_SIZE;
	//Allocate a buffer of length bufferLength
	buffer = _CL_NEWARRAY(UChar, bufferLength);
}

void StringBuffer::appendChar(const UChar character)
{
	//Func - Appends a single character
	//Pre  - true
	//Post - The character has been appended to the string in the buffer

	//Check if the current buffer length is sufficient to have the string value appended
	if (len + 1 > bufferLength) {
		//Have the size of the current string buffer increased because it is too small
		growBuffer(len + 1);
	}
	//Put character at position len which is the end of the string in the buffer
	//Note that this action might overwrite the terminator of the string '\0', which
	//is kind of tricky
	buffer[len] = character;
	//Increase the len by to represent the correct length of the string in the buffer
	len++;
}

void StringBuffer::append(const char *value)
{
	//Func - Appends a copy of the string value
	//Pre  - value != NULL
	//Post - value has been copied and appended to the string in buffer

	UnicodeString uStr(value);
	append(uStr.getBuffer(), uStr.length());
}

void StringBuffer::append(const UChar *value)
{
	//Func - Appends a copy of the string value
	//Pre  - value != NULL
	//Post - value has been copied and appended to the string in buffer

	append(value, _tcslen(value));
}

void StringBuffer::append(const UChar *value, size_t appendedLength)
{
	//Func - Appends a copy of the string value
	//Pre  - value != NULL
	//       appendedLength contains the length of the string value which is to be appended
	//Post - value has been copied and appended to the string in buffer

	//Check if the current buffer length is sufficient to have the string value appended
	if (len + (int32_t)appendedLength + 1 > bufferLength) {
		//Have the size of the current string buffer increased because it is too small
		growBuffer(len + appendedLength + 1);
	}

	//Copy the string value into the buffer at postion len
	_tcsncpy(buffer + len, value, appendedLength);

	//Add the length of the copied string to len to reflect the new length of the string in
	//the buffer (Note: len is not the bufferlength!)
	len += appendedLength;
}

void StringBuffer::appendInt(const int32_t value)
{
	//Func - Appends an integer (after conversion to a character string)
	//Pre  - true
	//Post - The converted integer value has been appended to the string in buffer

	//instantiate a buffer of 30 charactes for the conversion of the integer
	UChar buf[30];
	//Convert the integer value to a string buf using the radix 10 (duh)
	_i64tot(value, buf, 10);
	//Have the converted integer now stored in buf appended to the string in buffer
	append(buf);
}

void StringBuffer::appendFloat(const float_t value, const int32_t digits)
{
	//Func - Appends a float_t (after conversion to a character string)
	//Pre  - digits > 0. Indicates the minimum number of characters printed
	//Post - The converted float_t value has been appended to the string in buffer

	//using sprintf("%f" was not reliable on other plaforms... we use a custom float convertor
	//bvk: also, using sprintf and %f seems excessivelly slow
	if (digits > 8)
		_CLTHROWA(CL_ERR_IllegalArgument, "Too many digits...");

	//the maximum number of characters that int64 will hold is 23. so we need 23*2+2
	UChar buf[48]; //the buffer to hold
	int64_t v = (int64_t)value; //the integer value of the float
	_i64tot(v, buf, 10); //add the whole number

	size_t len = 99 - _tcslen(buf); //how many digits we have to work with?
	size_t dig = len < (size_t)digits ? len : digits;
	if (dig > 0) {
		_tcscat(buf, cStrDot); //add a decimal point

		int64_t remi = (int64_t)((value - v) * pow((float_t)10, (float_t)(dig + 1))); //take the remainder and make a whole number
		if (remi < 0) remi *= -1;
		int64_t remadj = remi / 10;
		if (remi - (remadj*10) >= 5)
			remadj++; //adjust remainder

		// add as many zeros as necessary between the decimal point and the
		// significant part of the number. Fixes a bug when trying to print
		// numbers that have zeros right after the decimal point
		if (remadj) {
			int32_t numZeros = dig - (int32_t)log10((float_t)remadj) - 1;
			while (numZeros-- > 0)
				_tcscat(buf, cStrZero); //add a zero before the decimal point
		}

		_i64tot(remadj, buf + _tcslen(buf), 10); //add the remainder
	}

	append(buf);
}

void StringBuffer::prepend(const UChar *value)
{
	//Func - Puts a copy of the string value infront of the current string in the StringBuffer
	//Pre  - value != NULL
	//Post - The string in pre(buffer) has been shifted n positions where n equals the length of value.
	//       The string value was then copied to the beginning of stringbuffer

	prepend(value, _tcslen(value));
}

void StringBuffer::prepend(const UChar *value, const size_t prependedLength)
{
	//Func - Puts a copy of the string value in front of the string in the StringBuffer
	//Pre  - value != NULL
	//       prependedLength contains the length of the string value which is to be prepended
	//Post - A copy of the string value is has been in front of the string in buffer
	//todo: something is wrong with this code, i'm sure... it only grows (and therefore moves if the buffer is to small)
	//Check if the current buffer length is sufficient to have the string value prepended
	if ((int32_t)prependedLength + len + 1 > bufferLength) {
		//Have the size of the current string buffer increased because it is too small
		//Because prependedLength is passed as the second argument to growBuffer,
		//growBuffer will have left the first prependedLength characters empty
		//when it recopied buffer during reallocation.
		growBuffer(prependedLength + len + 1, prependedLength);
	}

	//Copy the string value into the buffer at postion 0
	_tcsncpy(buffer, value, prependedLength);
	//Add the length of the copied string to len to reflect the new length of the string in
	//the buffer (Note: len is not the bufferlength!)
	len += prependedLength;
}

int32_t StringBuffer::length() const
{
	//Func - Returns the length of the string in the StringBuffer
	//Pre  - true
	//Post - The length len of the string in the buffer has been returned

	return len;
}

UChar *StringBuffer::toString()
{
	//Func - Returns a copy of the current string in the StringBuffer sized equal to the length of the string
	//       in the StringBuffer.
	//Pre  - true
	//Post - The copied string has been returned

	//Instantiate a buffer equal to the length len + 1
	UChar *ret = _CL_NEWARRAY(UChar, len + 1);
	if (ret) {
		//Copy the string in buffer
		_tcsncpy(ret, buffer, len);
		//terminate the string
		ret[len] = '\0';
	}
	//return the the copy
	return ret;
}

UChar *StringBuffer::getBuffer()
{
	//Func - '\0' terminates the buffer and returns its pointer
	//Pre  - true
	//Post - buffer has been '\0' terminated and returned

	// Check if the current buffer is '\0' terminated
	if (len == bufferLength) {
		//Make space for terminator, if necessary.
		growBuffer(len + 1);
	}
	//'\0' buffer so it can be returned properly
	buffer[len] = '\0';

	return buffer;
}

void StringBuffer::reserve(const int32_t size)
{
	if (bufferLength >= size)
		return;
	bufferLength = size;

	//Allocate a new buffer of length bufferLength
	UChar *tmp = _CL_NEWARRAY(UChar, bufferLength);
	_tcsncpy(tmp, buffer, len);
	tmp[len] = '\0';

	//destroy the old buffer
	if (buffer) {
		_CLDELETE_CARRAY(buffer);
	}
	//Assign the new buffer tmp to buffer
	buffer = tmp;
}

void StringBuffer::growBuffer(const int32_t minLength)
{
	//Func - Has the buffer grown to a minimum length of minLength or bigger
	//Pre  - minLength >= len + 1
	//Post - The buffer has been grown to a minimum length of minLength or bigger

	growBuffer(minLength, 0);
}

void StringBuffer::growBuffer(const int32_t minLength, const int32_t skippingNInitialChars)
{
	//Func - Has the buffer grown to a minimum length of minLength or bigger and shifts the
	//       current string in buffer by skippingNInitialChars forward
	//Pre  - After growth, must have at least enough room for contents + terminator so
	//       minLength >= skippingNInitialChars + len + 1
	//       skippingNInitialChars >= 0
	//Post - The buffer has been grown to a minimum length of minLength or bigger and
	//       if skippingNInitialChars > 0, the contents of the buffer has beeen shifted
	//       forward by skippingNInitialChars positions as the buffer is reallocated,
	//       leaving the first skippingNInitialChars uninitialized (presumably to be
	//       filled immediately thereafter by the caller).

	CND_PRECONDITION (skippingNInitialChars >= 0, "skippingNInitialChars is less than zero");
	CND_PRECONDITION (minLength >= skippingNInitialChars + len + 1, "skippingNInitialChars is not large enough");

	//More aggressive growth strategy to offset smaller default buffer size:
	if (!bufferOwner) {
		if (bufferLength < minLength)
			_CLTHROWA(CL_ERR_IllegalArgument, "[StringBuffer::grow] un-owned buffer could not be grown");
		return;
	}

	bufferLength *= 2;
	//Check that bufferLength is bigger than minLength
	if (bufferLength < minLength) {
		//Have bufferLength become minLength because it still was too small
		bufferLength = minLength;
	}

	//Allocate a new buffer of length bufferLength
	UChar *tmp = _CL_NEWARRAY(UChar, bufferLength);
	//The old buffer might not have been null-terminated, so we _tcsncpy
	//only len bytes, not len+1 bytes (the latter might read one char off the
	//end of the old buffer), then apply the terminator to the new buffer.
	_tcsncpy(tmp + skippingNInitialChars, buffer, len);
	tmp[skippingNInitialChars + len] = '\0';

	//destroy the old buffer
	if (buffer) {
		_CLDELETE_CARRAY(buffer);
	}
	//Assign the new buffer tmp to buffer
	buffer = tmp;
}

CL_NS_END
