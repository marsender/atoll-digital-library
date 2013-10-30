/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Zlib.cpp

*******************************************************************************/

#include "Zlib.hpp"
#include "Logger.hpp" // in common directory
#include "BinaryBuffer.hpp"
#include <zlib.h>
#ifndef WIN32
	#include <stdlib.h>
#endif
#define DEF_ZlibInterface
//------------------------------------------------------------------------------

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Zlib: %s", x); }
//------------------------------------------------------------------------------

#ifdef DEF_ZlibInterface

bool Common::ZlibCompress(BinaryBuffer &outBinaryBuffer, const char *inSrc, size_t inSrcSize)
{
	// No outBinaryBuffer reset !

	if (sizeof(Bytef) != sizeof(char)) {
		gLog.log(eTypLogError, "Err > Zlib: sizeof Bytef = '%d' != sizeof char = '%d'", sizeof(Bytef), sizeof(char));
		return false;
	}

	// Doc: http://www.zlib.net/manual.html#compress
	// int compress (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
	// Compresses the source buffer into the destination buffer.
	// sourceLen is the byte length of the source buffer.
	// Upon entry, destLen is the total size of the destination buffer,
	// which must be at least 0.1% larger than sourceLen plus 12 bytes.
	// Upon exit, destLen is the actual size of the compressed buffer.

	uLongf comprLen = compressBound((uLong)inSrcSize);
	Bytef *compr = (Bytef *)::malloc(sizeof(Bytef) * comprLen);

	// Call zlib compression utility function
	int err = compress(compr, &comprLen, (Bytef *)inSrc, (uLong)inSrcSize);
	if (err != Z_OK) {
		std::string error;
		switch(err) {
		case Z_MEM_ERROR:
			error = "Z_MEM_ERROR";
			break;
		case Z_BUF_ERROR:
			error = "Z_BUF_ERROR";
			break;
		default:
			error = "Unkonown error";
			break;
		}
		gLog.log(eTypLogError, "Err > Zlib: ZlibCompress -%s-", error.c_str());
	}

	outBinaryBuffer.write(compr, (size_t)comprLen);

	::free(compr);

	return (err == Z_OK);
}
//------------------------------------------------------------------------------

bool Common::ZlibDeflate(char *&outDest, unsigned int inDestSize, void *inData, size_t inDataSize)
{
	if (sizeof(Bytef) != sizeof(char)) {
		gLog.log(eTypLogError, "Err > Zlib: sizeof Bytef = '%d' != sizeof char = '%d'", sizeof(Bytef), sizeof(char));
		return false;
	}

	// Doc: http://www.zlib.net/manual.html#uncompress
	// int uncompress (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
	// Decompresses the source buffer into the destination buffer.
	// sourceLen is the byte length of the source buffer.
	// Upon entry, destLen is the total size of the destination buffer,
	// which must be large enough to hold the entire uncompressed data.
	// (The size of the uncompressed data must have been saved previously
	// by the compressor and transmitted to the decompressor by some mechanism
	// outside the scope of this compression library.)
	// Upon exit, destLen is the actual size of the uncompressed buffer. 

	// Call zlib decompression utility function
	uLongf uncomprLen = inDestSize;
	Bytef *uncompr = (Bytef *)::malloc(sizeof(Bytef) * inDestSize);
	int err = uncompress(uncompr, &uncomprLen, (Bytef *)inData, (uLong)inDataSize);
	if (err != Z_OK || inDestSize != uncomprLen) {
		std::string error;
		switch(err) {
		case Z_MEM_ERROR:
			error = "Z_MEM_ERROR";
			break;
		case Z_BUF_ERROR:
			error = "Z_BUF_ERROR";
			break;
		case Z_DATA_ERROR:
			error = "Z_DATA_ERROR";
			break;
		default:
			error = "Unkonown error";
			break;
		}
		if (inDestSize != uncomprLen)
			gLog.log(eTypLogError, "Err > Zlib: ZlibDeflate -%s- wanted size %u != uncompressed size %lu", error.c_str(), inDestSize, uncomprLen);
		else
			gLog.log(eTypLogError, "Err > Zlib: ZlibDeflate -%s-", error.c_str());
	}

	outDest = (char *)uncompr;

	return (err == Z_OK && inDestSize == uncomprLen);
}
//------------------------------------------------------------------------------

#else

enum {
	ZSTREAM_BUFFER_SIZE = 16384,
	ZSTREAM_GZIP        = 0x10,     // gzip header
	ZSTREAM_AUTO        = 0x20      // auto detect between gzip and zlib
};

bool Common::ZlibCompress(BinaryBuffer &outBinaryBuffer, const char *inSrc, size_t inSrcSize)
{
	unsigned char *m_z_buffer = new unsigned char[ZSTREAM_BUFFER_SIZE];
	struct z_stream_s *m_deflate = new z_stream_s;
	size_t m_z_size = ZSTREAM_BUFFER_SIZE;
	bool isOk = true;

	// Init
	{
		memset(m_deflate, 0, sizeof(z_stream_s));
		m_deflate->next_out = m_z_buffer;
		m_deflate->avail_out = (uInt)m_z_size;

		int windowBits;
		//windowBits = -MAX_WBITS; // Raw deflate stream, no header or checksum
		windowBits = MAX_WBITS; // Zlib header and checksum
		//windowBits = MAX_WBITS | ZSTREAM_GZIP; // Gzip header and checksum, requires zlib 1.2.1+

		// Compression levels:
		//   Z_NO_COMPRESSION Z_BEST_SPEED Z_BEST_COMPRESSION Z_DEFAULT_COMPRESSION
		int level = Z_BEST_SPEED;
		if (deflateInit2(m_deflate, level, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
			DEF_Log("Can't initialize zlib deflate stream.");
			isOk = false;
		}
	}

	// Compress
	if (isOk) {
		int err = Z_OK;
		m_deflate->next_in = (unsigned char *)inSrc;
		m_deflate->avail_in = (uInt)inSrcSize;

		while (err == Z_OK && m_deflate->avail_in > 0) {
			if (m_deflate->avail_out == 0) {
				if (outBinaryBuffer.write(m_z_buffer, m_z_size) != m_z_size) {
					DEF_Log("Error writing to underlying stream");
					isOk = false;
					break;
				}
				m_deflate->next_out = m_z_buffer;
				m_deflate->avail_out = (uInt)m_z_size;
			}
			err = deflate(m_deflate, Z_NO_FLUSH);
		}

		if (err != Z_OK) {
			DEF_Log("Can't write to stream");
			isOk = false;
		}
		inSrcSize -= m_deflate->avail_in;

		// Flush
		{
			int err = Z_OK;
			bool done = false;

			while (err == Z_OK || err == Z_STREAM_END) {
				size_t len = m_z_size  - m_deflate->avail_out;
				if (len) {
					if (outBinaryBuffer.write(m_z_buffer, len) != len) {
						DEF_Log("Error flushing to underlying stream");
						isOk = false;
						break;
					}
					m_deflate->next_out = m_z_buffer;
					m_deflate->avail_out = (uInt)m_z_size;
				}

				if (done)
					break;
				err = deflate(m_deflate, Z_FINISH);
				done = m_deflate->avail_out != 0 || err == Z_STREAM_END;
			}
		}
	}

	// Clean
	deflateEnd(m_deflate);
	delete m_deflate;
	delete[] m_z_buffer;

	return true;
}
//------------------------------------------------------------------------------

bool Common::ZlibDeflate(char *outDest, unsigned int inDestSize, void *inData, size_t inDataSize)
{
	bool isOk = true;
  struct z_stream_s *m_inflate = new z_stream_s;
  unsigned char *m_z_buffer = new unsigned char[ZSTREAM_BUFFER_SIZE];
  size_t m_z_size = ZSTREAM_BUFFER_SIZE;

	// Init
	{
		memset(m_inflate, 0, sizeof(z_stream_s));

		int windowBits = 0;
		//windowBits = -MAX_WBITS; // Raw deflate stream, no header or checksum
		windowBits = MAX_WBITS; // Zlib header and checksum
		//windowBits = MAX_WBITS | ZSTREAM_GZIP; // Gzip header and checksum, requires zlib 1.2.1+
		//windowBits = MAX_WBITS | ZSTREAM_AUTO; // Autodetect header zlib or gzip
		if (inflateInit2(m_inflate, windowBits) != Z_OK)
			isOk = false;
	}

	size_t sizeCurrent = 0;
	m_inflate->next_in = (Bytef *)inData;
	m_inflate->avail_in = (uInt)inDataSize;

  /*
	FILE *f = fopen("c:\\temp\\foo_unix_str.gz", "wb");
	fwrite(inStr, 1, atoi(inStr + 10), f);
  fclose(f);
	*/

	// Deflate
	size_t size;
	while (true) {
		int err = Z_OK;
		m_inflate->next_out = (unsigned char *)m_z_buffer;
		m_inflate->avail_out = (uInt)m_z_size;

		err = inflate(m_inflate, Z_SYNC_FLUSH);

		switch (err) {
		case Z_OK:
			break;
		case Z_STREAM_END:
			// Unread any data taken from past the end of the deflate stream, so that
			// any additional data can be read from the underlying stream (the crc
			// in a gzip for example)
			if (m_inflate->avail_in) {
				m_inflate->avail_in = 0;
			}
			break;
		case Z_BUF_ERROR:
			// Indicates that zlib was expecting more data, but the parent stream
			// has none. Other than Eof the error will have been already reported
			// by the parent strean,
			DEF_Log("Can't read inflate stream: unexpected EOF in underlying stream.");
			isOk = false;
			break;
		default:
			if (m_inflate->msg)
				gLog.log(eTypLogError, "Err > Zlib: Can't read from inflate stream. Error [%d]: %s", err, m_inflate->msg);
			else
				gLog.log(eTypLogError, "Err > Zlib: Can't read from inflate stream. Error [%d]", err);
			isOk = false;
			break;
		}

		size = m_z_size - m_inflate->avail_out;
		memcpy(outDest + sizeCurrent, m_z_buffer, size);
		sizeCurrent += size;
		outDest[sizeCurrent] = 0;

		if (!isOk || err == Z_STREAM_END)
			break;
	}

	if (sizeCurrent != inDestSize) {
		DEF_Log("Incorrect stream size");
		isOk = false;
	}

	// Ménage
  inflateEnd(m_inflate);
  delete m_inflate;
  delete [] m_z_buffer;

	return isOk;
}
//------------------------------------------------------------------------------
#endif // DEF_ZlibInterface
