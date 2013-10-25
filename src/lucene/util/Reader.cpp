/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "Reader.hpp"
#include <string.h>

CL_NS_DEF(util)

Reader::Reader(jsReader* reader, bool deleteReader) {
	this->reader = reader;
	this->deleteReader = deleteReader;
}

Reader::~Reader() {
	if (deleteReader)
		delete reader;
	reader = NULL;
}

int Reader::read() {
	const UChar*b;
	int32_t nread = reader->read(b, 1, 1);
	if (nread < -1) { //if not eof
		_CLTHROWA(CL_ERR_IO, reader->getError());
	}
	else if (nread == -1)
		return -1;
	else
		return b[0];
}

int32_t Reader::read(const UChar*& start) {
	int32_t nread = reader->read(start, 1, 0);
	if (nread < -1) { //if not eof
		_CLTHROWA(CL_ERR_IO, reader->getError());
	}
	else
		return nread;
}

int32_t Reader::read(const UChar*& start, int32_t len) {
	int32_t nread = reader->read(start, len, len);
	if (nread < -1) { //if not eof
		_CLTHROWA(CL_ERR_IO, reader->getError());
	}
	else
		return nread;
}

int64_t Reader::skip(int64_t ntoskip) {
	int64_t skipped = reader->skip(ntoskip);
	if (skipped < 0) {
		_CLTHROWA(CL_ERR_IO, reader->getError());
	}
	else
		return skipped;
}

int64_t Reader::mark(int32_t readAheadlimit) {
	int64_t pos = reader->mark(readAheadlimit);
	if (pos < 0) {
		_CLTHROWA(CL_ERR_IO, reader->getError());
	}
	else
		return pos;
}

int64_t Reader::reset(int64_t pos) {
	int64_t r = reader->reset(pos);
	if (r < 0) {
		_CLTHROWA(CL_ERR_IO, reader->getError());
	}
	else
		return r;
}

StringReader::StringReader(const UChar* value)
	:Reader(NULL, true)
{
	reader = new jstreams::StringReader<UChar>(value);
}

StringReader::StringReader(const UChar* value, const int32_t length)
	:Reader(NULL, true)
{
	reader = new jstreams::StringReader<UChar>(value, length);
}

StringReader::StringReader(const UChar* value, const int32_t length, bool copyData)
	:Reader(NULL, true)
{
	reader = new jstreams::StringReader<UChar>(value, length, copyData);
}

StringReader::~StringReader()
{
}

/*
FileReader::FileReader(const char* path, const char* enc,
                         const int32_t cachelen, const int32_t):
		Reader(NULL, true)
{
	this->input = new jstreams::FileInputStream(path, cachelen);
	this->reader = new SimpleInputStreamReader(this->input, enc); //(this is a jstream object)
}

FileReader::~FileReader()
{
	if (input)
		delete input;
}
int32_t FileReader::read(const UChar*& start, int32_t _min, int32_t _max)
{
	return reader->read(start, _min, _max);
}
int64_t FileReader::mark(int32_t readlimit)
{
	return reader->mark(readlimit);
}
int64_t FileReader::reset(int64_t newpos)
{
	return reader->reset(newpos);
}
*/

/*
SimpleInputStreamReader::SimpleInputStreamReader(jstreams::StreamBase<char> *i, const char* enc)
{
	finishedDecoding = false;
	input = i;
	charbuf.setSize(262);

	if (strcmp(enc, "ASCII") == 0)
		encoding = ASCII;
//#ifdef _UCS2
	else if (strcmp(enc, "UTF-8") == 0)
		encoding = UTF8;
	else if (strcmp(enc, "UCS-2LE") == 0)
		encoding = UCS2_LE;
//#endif
	else
		_CLTHROWA(CL_ERR_IllegalArgument, "Unsupported encoding, use jstreams iconv based instead");

	mark(262);
	charsLeft = 0;
}
SimpleInputStreamReader::~SimpleInputStreamReader()
{
	input = NULL;
}
int32_t SimpleInputStreamReader::decode(UChar* start, int32_t space)
{
	// decode from charbuf
	const char *inbuf = charbuf.readPos;
	const char *inbufend = charbuf.readPos + charbuf.avail;
	UChar *outbuf = start;
	const UChar *outbufend = outbuf + space;

	if (encoding == ASCII) {
		while (outbuf < outbufend && inbuf < inbufend) {
			*outbuf = *inbuf;
			outbuf++;
			inbuf++;
		}

//#ifdef _UCS2
	}
	else if (encoding == UCS2_LE) {
		while (outbuf < outbufend && (inbuf + 1) < inbufend) {
			uint8_t c1 = *inbuf;
			uint8_t c2 = *(inbuf + 1);
			unsigned short c = c1 | (c2 << 8);

//#ifdef _UCS2
			*outbuf = c;
//#else
//			*outbuf = LUCENE_OOR_CHAR(c);
//#endif
			outbuf++;
			inbuf += 2;
		}

	}
	else if (encoding == UTF8) {
		while (outbuf < outbufend && inbuf < inbufend) {
			size_t utflen = lucene_utf8charlen(inbuf);
			if (utflen == 0) {
				error = "Invalid multibyte sequence.";
				status = jstreams::Error;
				return -1;
			}
			else if (inbuf + utflen > inbufend) {
				break; //character incomplete
			}
			else {
				size_t rd = lucene_utf8towc(outbuf, inbuf, inbufend - inbuf);
				if (rd == 0) {
					error = "Invalid multibyte sequence.";
					status = jstreams::Error;
					return -1;
				}
				else {
					inbuf += rd;
					outbuf++;
				}
			}
		}
//#endif //_UCS2
	}
	else
		_CLTHROWA(CL_ERR_Runtime, "Unexpected encoding");

	if (outbuf < outbufend) {
		//we had enough room to convert the entire input
		if (inbuf < inbufend) {
			// last character is incomplete
			// move from inbuf to the end to the start of
			// the buffer
			memmove(charbuf.start, inbuf, inbufend - inbuf);
			charbuf.readPos = charbuf.start;
			charbuf.avail = inbufend - inbuf;
		}
		else if (outbuf < outbufend) { //input sequence was completely converted
			charbuf.readPos = charbuf.start;
			charbuf.avail = 0;
			if (input == NULL) {
				finishedDecoding = true;
			}
		}
	}
	else {
		charbuf.readPos += charbuf.avail - (inbufend - inbuf);
		charbuf.avail = inbufend - inbuf;
	}
	return outbuf -start;
}

int32_t SimpleInputStreamReader::fillBuffer(UChar* start, int32_t space)
{
	// fill up charbuf
	if (input && charbuf.readPos == charbuf.start) {
		const char *begin;
		int32_t numRead;
		numRead = input->read(begin, 1, charbuf.size - charbuf.avail);
		//printf("filled up charbuf\n");
		if (numRead < -1) {
			error = input->getError();
			status = jstreams::Error;
			input = 0;
			return numRead;
		}
		if (numRead < 1) {
			// signal end of input buffer
			input = 0;
			if (charbuf.avail) {
				error = "stream ends on incomplete character";
				status = jstreams::Error;
			}
			return -1;
		}
		// copy data into other buffer
		memmove(charbuf.start + charbuf.avail, begin, numRead * sizeof(char));
		charbuf.avail = numRead + charbuf.avail;
	}
	// decode
	int32_t n = decode(start, space);
	//printf("decoded %i\n", n);
	return n;
}
*/

CL_NS_END
