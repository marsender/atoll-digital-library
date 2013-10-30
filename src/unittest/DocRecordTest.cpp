/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocRecordTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DocRecordTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../util/DocRecord.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DocRecordTest, Atoll::UnitTestSuites::Util());

DocRecordTest::DocRecordTest()
{
}
//------------------------------------------------------------------------------


DocRecordTest::~DocRecordTest()
{
}
//------------------------------------------------------------------------------

void DocRecordTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void DocRecordTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void DocRecordTest::testSerialise(void)
{
	unsigned long pge = 1;
	unsigned long pos = DEF_LongMax;
	UnicodeString texte("Record content test");
	UnicodeString pageKey;

	// Init record
	DocRecord dr;
	dr.mPge = pge;
	dr.mPos = pos;
	dr.mTexte = texte;

	// Test
	dr.GetPageKey(pageKey);
	CPPUNIT_ASSERT(pageKey == "1");

	BinaryBuffer binaryBuffer;
	dr.ToBinaryBuffer(binaryBuffer);
	dr.Clear();
	dr.FromBinaryBuffer(binaryBuffer);

	CPPUNIT_ASSERT(dr.mPge == pge);
	CPPUNIT_ASSERT(dr.mPos == pos);
	CPPUNIT_ASSERT(dr.mTexte == texte);
}
//------------------------------------------------------------------------------
