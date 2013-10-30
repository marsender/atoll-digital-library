/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColMeta.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "ColMeta.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/Zlib.hpp"
//------------------------------------------------------------------------------

// Want zlib record compression / decompression ?
const bool cWantZlib = true;

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const ColMeta &inColMeta)
{
	inStream << inColMeta.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

ColMeta::ColMeta()
{
	mColNum = 0;
}
//------------------------------------------------------------------------------

void ColMeta::Clear()
{
	mColNum = 0;
	mColId.remove();
	mColTitle.remove();
	mColDesc.remove();
	mColSalt.remove();
}
//------------------------------------------------------------------------------

void ColMeta::Trim()
{
	UnicodeStringClearCrLf(mColTitle);
}
//------------------------------------------------------------------------------

void ColMeta::ToBinaryBuffer(BinaryBuffer &outBuffer) const
{
	size_t lgSize = DEF_SizeOfLong;
	char *buf = (char *)::malloc(lgSize);
	char *pos = buf;
	std::string str;
	unsigned long length;

	BinaryBuffer *binaryBuffer;
	if (cWantZlib)
		binaryBuffer = new BinaryBuffer;
	else {
		binaryBuffer = &outBuffer;
		binaryBuffer->resetCursorPosition();
	}

	SetLgBuf(pos, mColNum);
	pos += DEF_SizeOfLong;
	binaryBuffer->write(buf, lgSize);

	ConvertUnicodeString2String(str, mColId);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	ConvertUnicodeString2String(str, mColTitle);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	ConvertUnicodeString2String(str, mColDesc);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	ConvertUnicodeString2String(str, mColSalt);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	// Zlib compression
	if (cWantZlib) {
		length = (unsigned long)binaryBuffer->getOccupancy();
		SetLgBuf(buf, length);
		// Reset binary buffer
		outBuffer.reset();
		outBuffer.write(buf, DEF_SizeOfLong);
		ZlibCompress(outBuffer, (const char *)binaryBuffer->getBuffer(), length);
		delete binaryBuffer;
	}

	::free(buf);
}
//------------------------------------------------------------------------------

void ColMeta::FromBinaryBuffer(const BinaryBuffer &inBuffer)
{
	const char *pos = static_cast<const char*>(inBuffer.getBuffer());
	size_t size = inBuffer.getOccupancy();
	FromBinaryCharBuffer(pos, size);
}
//------------------------------------------------------------------------------

void ColMeta::FromBinaryCharBuffer(const char *inBuffer, size_t inSize)
{
	unsigned long length;

	// Zlib decompression
	char *uncompr = NULL;
	const char *pos = inBuffer;
	if (cWantZlib) {
		length = GetLgBuf(pos);
		pos += DEF_SizeOfLong;
		ZlibDeflate(uncompr, length, (void *)pos, inSize - DEF_SizeOfLong);
		pos = uncompr;
	}

	mColNum = GetLgBuf(pos);
	pos += DEF_SizeOfLong;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mColId, pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mColTitle, pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mColDesc, pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mColSalt, pos, length);
	pos += length;

	if (uncompr) {
		::free(uncompr);
	}
}
//------------------------------------------------------------------------------

std::string ColMeta::ToString() const
{
	std::string s("ColMeta: ");

	s += UnicodeString2String(mColId);
	s += " [";
	s += mColNum;
	s += "]";

	return s;
}
//------------------------------------------------------------------------------
