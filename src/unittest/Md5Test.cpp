/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Md5Test.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Md5Test.hpp"
#include "UnitTestSuites.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/Md5.hpp"
//------------------------------------------------------------------------------

using namespace std;
using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Md5Test, Atoll::UnitTestSuites::Common());

Md5Test::Md5Test()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


Md5Test::~Md5Test()
{
}
//------------------------------------------------------------------------------

void Md5Test::setUp(void)
{
}
//------------------------------------------------------------------------------

void Md5Test::tearDown(void)
{
}
//------------------------------------------------------------------------------

void Md5Test::testMain(void)
{
	std::string sMd5, sMd5Ok, mess;

	// String
	std::string sTest;
	sTest = "md5sum1_1 Word to search 1_1..md5sum1_2 Word to search 1_2";
	Md5String(sMd5, sTest);
	mess = "Md5 string: " + sMd5;
	sMd5Ok = "79be2f3810fcd6257d0dd3491146ea3b";
	CPPUNIT_ASSERT_MESSAGE("Md5 string", sMd5 == sMd5Ok);

	// Binary buffer
	BinaryBuffer binBuf;
	binBuf.write(sTest.c_str(), sTest.size());
	Md5BinaryBuffer(sMd5, binBuf);
	mess = "Md5 binary buffer: " + sMd5;
	CPPUNIT_ASSERT_MESSAGE("Md5 binary buffer", sMd5 == sMd5Ok);

	// File
	//   md5sum ./data_unittest/utf8_demo.xml
	std::string fileName = mPath + "utf8_demo.xml";
	Md5File(sMd5, fileName);
	mess = "Md5 file: " + sMd5;
	sMd5Ok = "93c12aef05e406c66ce9f6056303f9ea";
	CPPUNIT_ASSERT_MESSAGE(mess, sMd5 == sMd5Ok);
}
//------------------------------------------------------------------------------
