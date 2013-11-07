/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocMeta.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DocMeta.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/Zlib.hpp"
//------------------------------------------------------------------------------

// Want zlib record compression / decompression ?
const bool cWantZlib = true;

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const DocMeta &inDocMeta)
{
	inStream << inDocMeta.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

DocMeta::DocMeta(unsigned int inDocNum /*= 0*/)
{
	mDocNum = inDocNum;
	mCountPge = 0;
	mXmlValidation = false;
	mIsIndexed = false;
}
//------------------------------------------------------------------------------

void DocMeta::Clear()
{
	mDocNum = 0;
	mCountPge = 0;
	mXmlValidation = false;
	mIsIndexed = false;
	mDocTitle.remove();
	mUuid.remove();
	mXsltCode.remove();
	mFileName.clear();
	mDcMetadata.clear();
	mIndexerConfig.clear();
	mRecordBreakerConfig.clear();
}
//------------------------------------------------------------------------------

void DocMeta::Trim()
{
	UnicodeStringClearCrLf(mDocTitle);
}
//------------------------------------------------------------------------------

void DocMeta::ToBinaryBuffer(BinaryBuffer &outBuffer) const
{
	size_t lgSize = DEF_SizeOfLong + DEF_SizeOfLong + DEF_SizeOfInt + DEF_SizeOfInt;
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

	SetLgBuf(pos, mDocNum);
	pos += DEF_SizeOfLong;
	SetLgBuf(pos, mCountPge);
	pos += DEF_SizeOfLong;
	SetShBuf(pos, mXmlValidation);
	pos += DEF_SizeOfInt;
	SetShBuf(pos, mIsIndexed);
	pos += DEF_SizeOfInt;
	binaryBuffer->write(buf, lgSize);

	ConvertUnicodeString2String(str, mDocTitle);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	ConvertUnicodeString2String(str, mUuid);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	ConvertUnicodeString2String(str, mXsltCode);
	length = (unsigned long)str.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(str.c_str(), length);

	length = (unsigned long)mFileName.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(mFileName.c_str(), length);

	length = (unsigned long)mDcMetadata.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(mDcMetadata.c_str(), length);

	length = (unsigned long)mIndexerConfig.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(mIndexerConfig.c_str(), length);

	length = (unsigned long)mRecordBreakerConfig.length();
	SetLgBuf(buf, length);
	binaryBuffer->write(buf, DEF_SizeOfLong);
	binaryBuffer->write(mRecordBreakerConfig.c_str(), length);

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

void DocMeta::FromBinaryBuffer(const BinaryBuffer &inBuffer)
{
	const char *pos = static_cast<const char*>(inBuffer.getBuffer());
	size_t size = inBuffer.getOccupancy();
	FromBinaryCharBuffer(pos, size);
}
//------------------------------------------------------------------------------

void DocMeta::FromBinaryCharBuffer(const char *inBuffer, size_t inSize)
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

	mDocNum = GetLgBuf(pos);
	pos += DEF_SizeOfLong;

	mCountPge = GetLgBuf(pos);
	pos += DEF_SizeOfLong;

	mXmlValidation = GetShBuf(pos);
	pos += DEF_SizeOfInt;

	mIsIndexed = GetShBuf(pos);
	pos += DEF_SizeOfInt;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mDocTitle, pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mUuid, pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	ConvertString2UnicodeString(mXsltCode, pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	mFileName.assign(pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	mDcMetadata.assign(pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	mIndexerConfig.assign(pos, length);
	pos += length;

	length = GetLgBuf(pos);
	pos += DEF_SizeOfLong;
	mRecordBreakerConfig.assign(pos, length);
	pos += length;

	if (uncompr) {
		::free(uncompr);
	}
}
//------------------------------------------------------------------------------

void DocMeta::ToXml(UnicodeString &outStr) const
{
	outStr.remove();

	// Metadata search fields, as in the metadata indexer config file
	//   ./config/atoll_indexer_config_metadata.xml
	outStr += "<docmeta xmlns:atoll=\"urn:atoll\" xmlns:metadata=\"urn:atoll:metadata\">";
	outStr += "	<metadata:title>"; UnicodeStringAddXmlEntities(outStr, mDocTitle); outStr += "</metadata:title>";
	outStr += "	<metadata:uuid>"; UnicodeStringAddXmlEntities(outStr, mUuid); outStr += "</metadata:uuid>";
	outStr += "</docmeta>";

	/* Rdf version
	outStr += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	outStr += "<!DOCTYPE rdf:RDF PUBLIC \"-//DUBLIN CORE//DCMES DTD 2002/07/31//EN\"\n";
	outStr += "    \"http://dublincore.org/documents/2002/07/31/dcmes-xml/dcmes-xml-dtd.dtd\">\n";
	outStr += "<rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n";
	outStr += "         xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n";
	outStr += "	<rdf:Description rdf:about=\"http://www.ilrt.bristol.ac.uk/people/cmdjb/\">\n";
	outStr += "		<dc:title>"; UnicodeStringAddXmlEntities(outStr, mDocTitle); outStr += "</dc:title>\n";
	outStr += "		<dc:creator>"; outStr += "Didier Corbière"; outStr += "</dc:creator>\n";
	outStr += "	</rdf:Description>\n";
	outStr += "</rdf:RDF>\n";
	*/
}
//------------------------------------------------------------------------------

std::string DocMeta::ToString() const
{
	std::string s("DocMeta: ");

	s += UnicodeString2String(mDocTitle);
	s += " [";
	s += mDocNum;
	s += "]";

	return s;
}
//------------------------------------------------------------------------------
