/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocRecord.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DocRecord.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/Zlib.hpp"
//------------------------------------------------------------------------------

// Want zlib record compression / decompression ?
const bool cWantZlib = true;

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const DocRecord &inDocRecord)
{
	inStream << inDocRecord.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

DocRecord::DocRecord()
{
	mPge = 0;
	mPos = 0;
}
//------------------------------------------------------------------------------

void DocRecord::Clear()
{
	mPge = 0;
	mPos = 0;
	mTexte.remove();
}
//------------------------------------------------------------------------------

void DocRecord::GetPageKey(UnicodeString &outKey) const
{
	Common::UnicodeStringSetULong(outKey, mPge);
}
//------------------------------------------------------------------------------

void DocRecord::ToBinaryBuffer(BinaryBuffer &outBuffer) const
{
	size_t lgSize = 2 * DEF_SizeOfLong;
	char *buf = static_cast<char*>(::malloc(lgSize));
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

	SetLgBuf(pos, mPge);
	pos += DEF_SizeOfLong;
	SetLgBuf(pos, mPos);
	pos += DEF_SizeOfLong;
	binaryBuffer->write(buf, lgSize);

	ConvertUnicodeString2String(str, mTexte);
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

void DocRecord::FromBinaryBuffer(const BinaryBuffer &inBuffer)
{
	const char *pos = static_cast<const char*>(inBuffer.getBuffer());
	size_t size = inBuffer.getOccupancy();
	FromBinaryCharBuffer(pos, size);
}
//------------------------------------------------------------------------------

void DocRecord::FromBinaryCharBuffer(const char *inBuffer, size_t inSize)
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

	mPge = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	mPos = GetLgBuf(pos);
	pos += DEF_SizeOfLong;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mTexte, pos, length);
	pos += length;

	if (uncompr) {
		::free(uncompr);
	}
}
//------------------------------------------------------------------------------

std::string DocRecord::ToString() const
{
	std::string s("DocRecord: ");

	s += UnicodeString2String(mTexte);
	s += " [";
	//s += mEntry.ToString();
	s += "]";

	return s;
}
//------------------------------------------------------------------------------
