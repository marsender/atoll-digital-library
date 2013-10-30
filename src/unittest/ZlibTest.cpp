/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ZlibTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "ZlibTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/Zlib.hpp"
#include <zlib.h>
//------------------------------------------------------------------------------

using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ZlibTest, Atoll::UnitTestSuites::Common());

ZlibTest::ZlibTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


ZlibTest::~ZlibTest()
{
}
//------------------------------------------------------------------------------

void ZlibTest::setUp()
{
}
//------------------------------------------------------------------------------

void ZlibTest::tearDown()
{
}
//------------------------------------------------------------------------------

void ZlibTest::testCompress()
{
	int err;
	const char hello[] = "hello, hello!";

	unsigned long uncomprLen = (unsigned long)strlen(hello) + 1;
	unsigned long comprLen = compressBound(uncomprLen);

	Bytef *compr = (Bytef *)::malloc(sizeof(Bytef) * comprLen);
	CPPUNIT_ASSERT(compr != Z_NULL);
	err = compress(compr, &comprLen, (Bytef *)hello, uncomprLen);
	CPPUNIT_ASSERT_MESSAGE("Zlib: Compress error", err == Z_OK);

	Bytef *uncompr = (Bytef *)::malloc(sizeof(Bytef) * uncomprLen);
	CPPUNIT_ASSERT(uncompr != Z_NULL);
	err = uncompress(uncompr, &uncomprLen, compr, comprLen);
	CPPUNIT_ASSERT_MESSAGE("Zlib: Uncompress error", err == Z_OK);
	CPPUNIT_ASSERT_MESSAGE("Zlib: Bad uncompress", strcmp((char *)uncompr, hello) == 0);

	::free(compr);
	::free(uncompr);
}
//------------------------------------------------------------------------------

void ZlibTest::testZlibCompress()
{
	bool isOk;
	const char hello[] = "hello, hello!";
	unsigned int uncomprLen = (unsigned int)strlen(hello) + 1;

	BinaryBuffer binaryBuffer;
	isOk = ZlibCompress(binaryBuffer, hello, uncomprLen);
	CPPUNIT_ASSERT(isOk);

	char *uncompr = NULL;
	isOk = ZlibDeflate(uncompr, uncomprLen, binaryBuffer.getBuffer(), binaryBuffer.getOccupancy());
	CPPUNIT_ASSERT(isOk);

	CPPUNIT_ASSERT_MESSAGE("Zlib: Bad uncompress", strcmp(uncompr, hello) == 0);

	::free(uncompr);
}
//------------------------------------------------------------------------------

void ZlibTest::testZlibCompressBinary()
{
	bool isOk;
	unsigned long length;
	unsigned long num, num1 = 0, num2 = 1640;
	std::string str, str1, str2, str3("hello");
	BinaryBuffer compressBuffer;

	unsigned long contentSize = 5 * DEF_SizeOfLong + str1.length() + str2.length() + str3.length();
	CPPUNIT_ASSERT_MESSAGE("Bad binary buffer size", contentSize == 25);

	{
		size_t lgSize = DEF_SizeOfLong + DEF_SizeOfLong;
		BinaryBuffer binaryBuffer;
		char *buf = (char *)::malloc(lgSize);
		char *pos = buf;

		// Store some numbers
		SetLgBuf(pos, num1);
		pos += DEF_SizeOfLong;
		SetLgBuf(pos, num2);
		pos += DEF_SizeOfLong;
		binaryBuffer.write(buf, lgSize);

		// Store a string
		length = (unsigned long)str1.length();
		SetLgBuf(buf, length);
		binaryBuffer.write(buf, DEF_SizeOfLong);
		binaryBuffer.write(str1.c_str(), length);

		// Store a string
		length = (unsigned long)str2.length();
		SetLgBuf(buf, length);
		binaryBuffer.write(buf, DEF_SizeOfLong);
		binaryBuffer.write(str2.c_str(), length);

		// Store a string
		length = (unsigned long)str3.length();
		SetLgBuf(buf, length);
		binaryBuffer.write(buf, DEF_SizeOfLong);
		binaryBuffer.write(str3.c_str(), length);

		// Zlib compression
		length = (unsigned long)binaryBuffer.getOccupancy();
		CPPUNIT_ASSERT_MESSAGE("Bad binary buffer occupancy", length == contentSize);
		SetLgBuf(buf, length);
		// Reset binary buffer
		compressBuffer.reset();
		compressBuffer.write(buf, DEF_SizeOfLong);
		isOk = ZlibCompress(compressBuffer, (const char *)binaryBuffer.getBuffer(), length);
		CPPUNIT_ASSERT_MESSAGE("Bad zlib binary compression", isOk);

		::free(buf);
	}

	{
		// Zlib deflate
		const char *pos = static_cast<const char*>(compressBuffer.getBuffer());
		size_t size = compressBuffer.getOccupancy();
		CPPUNIT_ASSERT_MESSAGE("Bad compress buffer occupancy", size == 26);
		length = GetLgBuf(pos);
		pos += DEF_SizeOfLong;
		CPPUNIT_ASSERT_MESSAGE("Bad binary buffer occupancy", length == contentSize);
		char *uncompr = NULL;
		isOk = ZlibDeflate(uncompr, length, (void *)pos, size - DEF_SizeOfLong);
		CPPUNIT_ASSERT_MESSAGE("Bad zlib binary delate", isOk);
		pos = uncompr;

		// Get the numbers
		num = GetLgBuf(pos);
		pos += DEF_SizeOfLong;
		CPPUNIT_ASSERT_MESSAGE("Bad number 1", num == num1);
		num = GetLgBuf(pos);
		pos += DEF_SizeOfLong;
		CPPUNIT_ASSERT_MESSAGE("Bad number 2", num == num2);

		// Get string
		length = GetLgBuf(pos);
		pos += DEF_SizeOfLong;
		str.assign(pos, length);
		pos += length;
		CPPUNIT_ASSERT_MESSAGE("Bad string 1", str == str1);

		// Get string
		length = GetLgBuf(pos);
		pos += DEF_SizeOfLong;
		str.assign(pos, length);
		pos += length;
		CPPUNIT_ASSERT_MESSAGE("Bad string 2", str == str2);

		// Get string
		length = GetLgBuf(pos);
		pos += DEF_SizeOfLong;
		str.assign(pos, length);
		pos += length;
		CPPUNIT_ASSERT_MESSAGE("Bad string 3", str == str3);

		::free(uncompr);
	}
}
//------------------------------------------------------------------------------

void ZlibTest::testZlibCompressFileContent()
{
	ZlibCompressFileContent(NULL);
	ZlibCompressFileContent("SCSU");
	ZlibCompressFileContent("utf-8");
}
//------------------------------------------------------------------------------

void ZlibTest::ZlibCompressFileContent(const char *inConverterName)
{
	bool isOk;
	std::string fileName = mPath + "utf8_demo.xml";

	UnicodeString uStr;
	isOk = File2UnicodeString(uStr, fileName.c_str());
	CPPUNIT_ASSERT(isOk);

	std::string str;
	isOk = ConvertUnicodeString2String(str, uStr, inConverterName);
	CPPUNIT_ASSERT(isOk);

	BinaryBuffer binaryBuffer;
	isOk = ZlibCompress(binaryBuffer, str.c_str(), str.length());
	CPPUNIT_ASSERT(isOk);

	unsigned int uncomprLen = (unsigned int)str.length();
	char *uncompr = NULL;
	isOk = ZlibDeflate(uncompr, uncomprLen, binaryBuffer.getBuffer(), binaryBuffer.getOccupancy());
	CPPUNIT_ASSERT(isOk);
	CPPUNIT_ASSERT_MESSAGE("Zlib: Bad uncompress", strncmp(uncompr, str.c_str(), uncomprLen) == 0);
	::free(uncompr);

	UnicodeString uStr2;
	isOk = ConvertString2UnicodeString(uStr2, str, inConverterName);
	CPPUNIT_ASSERT(isOk);

	CPPUNIT_ASSERT_MESSAGE("UnicodeString: Bad compression", uStr2 == uStr);
}
//------------------------------------------------------------------------------
