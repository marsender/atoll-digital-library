/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

SearchRecord.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "SearchRecord.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/Md5.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/Zlib.hpp"
//------------------------------------------------------------------------------

// Want zlib record compression / decompression ?
const bool cWantZlib = true;

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const SearchRecord &inSearchRecord)
{
	inStream << inSearchRecord.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

SearchRecord::SearchRecord()
{
	mTypSearchRecord = eTypSearchRecordNone;
	mValidityIndice = 0; // Not valid
}
//------------------------------------------------------------------------------

void SearchRecord::Clear()
{
	mTypSearchRecord = eTypSearchRecordNone;
	mValidityIndice = 0; // Not valid
	mSearchId.clear();
	mSearchCritVector.clear();
	mEntrySet.clear();
}
//------------------------------------------------------------------------------

void SearchRecord::GetSearchRecordKey(UnicodeString &outKey) const
{
	Common::UnicodeStringSetULong(outKey, mTypSearchRecord);
	outKey += " ";
	outKey += mSearchId.c_str();
}
//------------------------------------------------------------------------------

/*
void SearchRecord::SetSearchId()
{
	BinaryBuffer buffer;

	SearchCritVectorToBuf(buffer, mSearchCritVector);
	Md5BinaryBuffer(mSearchId, buffer);
}
//------------------------------------------------------------------------------
*/

bool SearchRecord::IsValid() const
{
	if (mTypSearchRecord <= eTypSearchRecordNone || mTypSearchRecord >= eTypSearchRecordEnd) {
		gLog.log(eTypLogError, "Err > SearchRecord: Inocrrect search record type");
		return false;
	}

	if (mSearchId.length() != 32) {
		gLog.log(eTypLogError, "Err > SearchRecord: Search id is not an md5 sum");
		return false;
	}

	if (mSearchCritVector.size() == 0) {
		gLog.log(eTypLogError, "Err > SearchRecord: Empty search criteria vector");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

void SearchRecord::ToBinaryBuffer(BinaryBuffer &outBuffer) const
{
	size_t lgSize = 2 * DEF_SizeOfInt; // + DEF_SizeOfLong;
	char *buf = (char *)::malloc(lgSize);
	char *pos = buf;
	unsigned long length;

	BinaryBuffer *binaryBuffer;
	if (cWantZlib)
		binaryBuffer = new BinaryBuffer;
	else {
		binaryBuffer = &outBuffer;
		binaryBuffer->resetCursorPosition();
	}

	// Serialize the search criteria vector
	BinaryBuffer binBuf;
	SearchCritVectorToBuf(binBuf, mSearchCritVector);
	// Init the search id with the search criteria vector md5 sum
	std::string &strMd5 = (std::string &)mSearchId; // ConstHack
	Md5BinaryBuffer(strMd5, binBuf);

	SetShBuf(pos, mTypSearchRecord);
	pos += DEF_SizeOfInt;
	SetShBuf(pos, mValidityIndice);
	pos += DEF_SizeOfInt;
	binaryBuffer->write(buf, lgSize);

	length = (unsigned long)mSearchId.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(mSearchId.c_str(), length);

	unsigned long searchCritVectorSize = mSearchCritVector.size();
	if (searchCritVectorSize == 0) {
		SetLgBuf(buf, 0);
		binaryBuffer->write(buf, DEF_SizeOfLong);
	}
	else {
		length = binBuf.getOccupancy();
		SetLgBuf(buf, length);
		binaryBuffer->write(buf, DEF_SizeOfLong);
		binaryBuffer->write(binBuf.getBuffer(), length);
	}

	unsigned long entrySetSize = mEntrySet.size();
	if (entrySetSize == 0) {
		SetLgBuf(buf, 0);
		binaryBuffer->write(buf, DEF_SizeOfLong);
	}
	else {
		size_t dataSize;
		BinaryBuffer binBuf;
		EntrySetToBuf(binBuf, dataSize, mEntrySet);
		length = binBuf.getOccupancy();
		SetLgBuf(buf, length);
		binaryBuffer->write(buf, DEF_SizeOfLong);
		binaryBuffer->write(binBuf.getBuffer(), length);
	}

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

void SearchRecord::FromBinaryBuffer(const BinaryBuffer &inBuffer)
{
	const char *pos = static_cast<const char*>(inBuffer.getBuffer());
	size_t size = inBuffer.getOccupancy();
	FromBinaryCharBuffer(pos, size);
}
//------------------------------------------------------------------------------

void SearchRecord::FromBinaryCharBuffer(const char *inBuffer, size_t inSize)
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

	mTypSearchRecord = (eTypSearchRecord)GetShBuf(pos);
	pos += DEF_SizeOfInt;

	mValidityIndice = GetShBuf(pos);
	pos += DEF_SizeOfInt;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	mSearchId.assign(pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	if (length == 0) {
		mSearchCritVector.clear();
	}
	else {
		BufToSearchCritVector(mSearchCritVector, pos);
		pos += length;
	}

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	if (length == 0) {
		mEntrySet.clear();
	}
	else {
		BufToEntrySet(mEntrySet, pos, length, NULL);
		pos += length;
	}

	if (uncompr) {
		::free(uncompr);
	}
}
//------------------------------------------------------------------------------

std::string SearchRecord::ToString() const
{
	std::string s("SearchRecord: ");

	s += mTypSearchRecord;
	s += " [";
	s += mSearchId;
	s += "] ";

	return s;
}
//------------------------------------------------------------------------------
