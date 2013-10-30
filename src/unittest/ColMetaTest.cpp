/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColMetaTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "ColMetaTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../util/ColMeta.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ColMetaTest, Atoll::UnitTestSuites::Util());

ColMetaTest::ColMetaTest()
{
}
//------------------------------------------------------------------------------


ColMetaTest::~ColMetaTest()
{
}
//------------------------------------------------------------------------------

void ColMetaTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void ColMetaTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void ColMetaTest::testSerialise(void)
{
	unsigned int num = DEF_IntMax;
	UnicodeString id("Id of the collection");
	UnicodeString title("Title of the collection");
	UnicodeString desc("Description of the collection");
	UnicodeString salt("Salt of the collection");

	// Init record
	ColMeta cm;
	cm.mColNum = num;
	cm.mColId = id;
	cm.mColTitle = title;
	cm.mColDesc = desc;
	cm.mColSalt = salt;

	BinaryBuffer binaryBuffer;
	cm.ToBinaryBuffer(binaryBuffer);
	cm.Clear();
	cm.FromBinaryBuffer(binaryBuffer);

	CPPUNIT_ASSERT(cm.mColNum == num);
	CPPUNIT_ASSERT(cm.mColId == id);
	CPPUNIT_ASSERT(cm.mColTitle == title);
	CPPUNIT_ASSERT(cm.mColDesc == desc);
	CPPUNIT_ASSERT(cm.mColSalt == salt);
}
//------------------------------------------------------------------------------
