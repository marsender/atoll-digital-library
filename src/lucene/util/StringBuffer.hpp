/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_util_StringBuffer_
#define _lucene_util_StringBuffer_

//! Unicode string buffer, that may be replaced by UnicodeString
CL_NS_DEF(util)
class StringBuffer
{
public:
	///Constructor. Allocates a buffer with the default length.
	StringBuffer();
	///Constructor. Allocates a buffer of length initSize + 1
	StringBuffer(const int32_t initSize);
	///Constructor. Creates an instance of Stringbuffer containing a copy of
	///the string value
	StringBuffer(const UChar *value);
	///Constructs a StringBuffer using another buffer. The StringBuffer can
	///the be used to easily manipulate the buffer.
	StringBuffer(UChar *buf, int32_t maxlen, const bool consumeBuffer);
	///Destructor
	~StringBuffer();

	//! Init string declarations
	static void InitStringDecl();

	///Clears the Stringbuffer and resets it to it default empty state
	void clear();

	///Appends a single character
	void appendChar(const UChar chr);
	///Appends a copy of the string value
	void append(const char *value);
	///Appends a copy of the string value
	void append(const UChar *value);
	///Appends a copy of the string value
	void append(const UChar *value, size_t appendedLength);
	///Appends an integer (after conversion to a character string)
	void appendInt(const int32_t value);
	///Appends a float_t (after conversion to a character string)
	void appendFloat(const float_t value, const int32_t digits);
	///Puts a copy of the string value in front of the current string in the StringBuffer
	void prepend(const UChar *value);
	///Puts a copy of the string value in front of the current string in the StringBuffer
	void prepend(const UChar *value, size_t prependedLength);

	///Contains the length of string in the StringBuffer
	///Public so that analyzers can edit the length directly
	int32_t len;
	///Returns the length of the string in the StringBuffer
	int32_t length() const;
	///Returns a copy of the current string in the StringBuffer
	UChar *toString();
	///Returns a null terminated reference to the StringBuffer's text
	UChar *getBuffer();


	///reserve a minimum amount of data for the buffer.
	///no change made if the buffer is already longer than length
	void reserve(const int32_t length);
private:
	///A buffer that contains strings
	UChar *buffer;
	///The length of the buffer
	int32_t bufferLength;
	bool bufferOwner;

	///Has the buffer grown to a minimum length of minLength or bigger
	void growBuffer(const int32_t minLength);
	///Has the buffer grown to a minimum length of minLength or bigger and shifts the
	///current string in buffer by skippingNInitialChars forward
	void growBuffer(const int32_t minLength, const int32_t skippingNInitialChars);

};
CL_NS_END
#endif
