/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

FieldMeta.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "FieldMeta.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/Zlib.hpp"
//------------------------------------------------------------------------------

// Want zlib record compression / decompression ?
const bool cWantZlib = true;

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const FieldMeta &inFieldMeta)
{
	inStream << inFieldMeta.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

FieldMeta::FieldMeta()
{
	mTypField = eTypFieldNone;
	mFieldOrder = 0;
}
//------------------------------------------------------------------------------

void FieldMeta::Clear()
{
	mTypField = eTypFieldNone;
	mFieldOrder = 0;
	mFieldId.remove();
	mFieldTitle.remove();
}
//------------------------------------------------------------------------------

void FieldMeta::Trim()
{
	UnicodeStringClearCrLf(mFieldId);
	UnicodeStringClearCrLf(mFieldTitle);
}
//------------------------------------------------------------------------------

UnicodeString FieldMeta::GetFieldId() const
{
	return mFieldId;
}
//------------------------------------------------------------------------------

void FieldMeta::SetFieldId(const UnicodeString &inId)
{
	mFieldId = inId;
}
//------------------------------------------------------------------------------

void FieldMeta::GetFieldKey(UnicodeString &outKey) const
{
	Common::UnicodeStringSetULong(outKey, mTypField);
	outKey += " ";
	outKey += mFieldId;
}
//------------------------------------------------------------------------------

void FieldMeta::GetStrFieldType(std::string &outStrFieldType) const
{
	switch (mTypField) {
	case eTypFieldAll: outStrFieldType = "all"; break;
	case eTypFieldSearch: outStrFieldType = "search"; break;
	default: outStrFieldType.erase(); break;
	}
}
//------------------------------------------------------------------------------

void FieldMeta::SetFieldType(const std::string &inStrFieldType)
{
	if (inStrFieldType == "all")
		mTypField = eTypFieldAll;
	else if ("search")
		mTypField = eTypFieldSearch;
	else
		mTypField = eTypFieldNone;
}
//------------------------------------------------------------------------------

bool FieldMeta::IsValid() const
{
	if (mTypField <= eTypFieldNone || mTypField >= eTypFieldEnd) {
		gLog.log(eTypLogError, "Err > FieldMeta: Inocrrect field type");
		return false;
	}

	if (mFieldId.isEmpty()) {
		gLog.log(eTypLogError, "Err > FieldMeta: Empty field id");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

void FieldMeta::ToBinaryBuffer(BinaryBuffer &outBuffer) const
{
	size_t lgSize = 2 * DEF_SizeOfInt;
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

	SetShBuf(pos, mTypField);
	pos += DEF_SizeOfInt;
	SetShBuf(pos, mFieldOrder);
	pos += DEF_SizeOfInt;
	binaryBuffer->write(buf, lgSize);

	ConvertUnicodeString2String(str, mFieldId);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	ConvertUnicodeString2String(str, mFieldTitle);
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

void FieldMeta::FromBinaryBuffer(const BinaryBuffer &inBuffer)
{
	const char *pos = static_cast<const char*>(inBuffer.getBuffer());
	size_t size = inBuffer.getOccupancy();
	FromBinaryCharBuffer(pos, size);
}
//------------------------------------------------------------------------------

void FieldMeta::FromBinaryCharBuffer(const char *inBuffer, size_t inSize)
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

	mTypField = (eTypField)GetShBuf(pos);
	pos += DEF_SizeOfInt;

	mFieldOrder = GetShBuf(pos);
	pos += DEF_SizeOfInt;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mFieldId, pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mFieldTitle, pos, length);
	pos += length;

	if (uncompr) {
		::free(uncompr);
	}
}
//------------------------------------------------------------------------------

std::string FieldMeta::ToString() const
{
	std::string s("FieldMeta: ");

	s += mTypField;
	s += " [";
	s += UnicodeString2String(mFieldId);
	s += " ";
	s += mFieldOrder;
	s += "] ";
	s += UnicodeString2String(mFieldTitle);

	return s;
}
//------------------------------------------------------------------------------
