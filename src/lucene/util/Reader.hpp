/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_util_Reader_
#define _lucene_util_Reader_

#include "StreamBase.hpp"
#include "StringReader.hpp"
//#include "fileinputstream.h"
//#include "bufferedstream.h"

CL_NS_DEF(util)

/**
* An inline wrapper that reads from Jos van den Oever's jstreams
*/
class Reader
{
	typedef jstreams::StreamBase<UChar> jsReader;
public:
	bool deleteReader;
	jsReader* reader;

	Reader(jsReader* reader, bool deleteReader);
	virtual ~Reader();

	int read();
	/**
	* Read at least 1 character, and as much as is conveniently available
	*/
	int32_t read(const UChar*& start);
	int32_t read(const UChar*& start, int32_t len);
	int64_t skip(int64_t ntoskip);
	int64_t mark(int32_t readAheadlimit);
	int64_t reset(int64_t pos);
};

///A helper class which constructs a the jstreams StringReader.
class StringReader: public Reader
{
public:
	StringReader(const UChar* value);
	StringReader(const UChar* value, const int32_t length);
	StringReader(const UChar* value, const int32_t length, bool copyData);
	~StringReader();
};

/** A very simple inputstreamreader implementation. For a
* more complete InputStreamReader, use the jstreams version
* located in the contrib package
*/
/*
class SimpleInputStreamReader: public jstreams::BufferedInputStream<UChar>
{
	int32_t decode(UChar* start, int32_t space);
	int encoding;
	enum {
		ASCII = 1,
		UTF8 = 2,
		UCS2_LE = 3
	};
	bool finishedDecoding;
	jstreams::StreamBase<char>* input;
	int32_t charsLeft;

	jstreams::InputStreamBuffer<char> charbuf;
	int32_t fillBuffer(UChar* start, int32_t space);
public:
	SimpleInputStreamReader(jstreams::StreamBase<char> *i, const char* encoding);
	~SimpleInputStreamReader();
};
*/

/**
* A helper class which constructs a FileReader with a specified
* simple encodings, or a given inputstreamreader
*/
/*
class FileReader: public Reader
{
	jstreams::FileInputStream* input;
public:
	FileReader(const char* path, const char* enc,
	           const int32_t cachelen = 13,
	           const int32_t cachebuff = 14); // todo: optimise these cache values
	~FileReader();

	int32_t read(const UChar*& start, int32_t _min, int32_t _max);
	int64_t mark(int32_t readlimit);
	int64_t reset(int64_t);
};
*/

CL_NS_END
#endif
