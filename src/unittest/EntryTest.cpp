/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EntryTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "EntryTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../util/Entry.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(EntryTest, Atoll::UnitTestSuites::Engine());

EntryTest::EntryTest()
{
}
//------------------------------------------------------------------------------


EntryTest::~EntryTest()
{
}
//------------------------------------------------------------------------------

void EntryTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void EntryTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void EntryTest::testEmpty(void)
{
	Entry e;

  CPPUNIT_ASSERT(e.IsEmpty());
	e.Set(0, 0, 0);
  CPPUNIT_ASSERT(e.IsEmpty());
	e.Set(0, 0, 1);
  CPPUNIT_ASSERT(e.IsEmpty() == false);
	e.Set(0, 1, 0);
  CPPUNIT_ASSERT(e.IsEmpty() == false);
	e.Set(1, 0, 0);
  CPPUNIT_ASSERT(e.IsEmpty() == false);
}
//------------------------------------------------------------------------------

void EntryTest::testDppMax(void)
{
  CPPUNIT_ASSERT(DEF_IntMax == 0xffff);
  CPPUNIT_ASSERT(DEF_IntMax == 65535);

  CPPUNIT_ASSERT(DEF_LongMax == 0xffffffff);
  CPPUNIT_ASSERT(DEF_LongMax == 4294967295UL); // Unsigned long

	unsigned int uiTest = DEF_IntMax;
	CPPUNIT_ASSERT(uiTest == DEF_IntMax);

	unsigned long ulTest = DEF_LongMax;
	CPPUNIT_ASSERT(ulTest == DEF_LongMax);
}
//------------------------------------------------------------------------------

void EntryTest::testBinaryStorage(void)
{
	Entry e1, e2;
	char buf[DEF_SizeOfEntry];

	// Set entry and store it into the binary buffer
	e1.Set(DEF_IntMax, DEF_LongMax, DEF_LongMax);
	NotInlineEntryToBuf(buf, &e1);

	// Set new entry from the binary buffer
	e2.Set(0, 0, 0);
	NotInlineBufToEntry(buf, &e2);

  CPPUNIT_ASSERT(e2.mUiEntDoc == DEF_IntMax);
  CPPUNIT_ASSERT(e2.mUlEntPge == DEF_LongMax);
  CPPUNIT_ASSERT(e2.mUlEntPos == DEF_LongMax);
}
//------------------------------------------------------------------------------

void EntryTest::testStringStorage(void)
{
	Entry e1, e2;
	char bufScan[DEF_SizeOfEntryScan];

	// Set entry and store it into the string buffer
	e1.Set(DEF_IntMax, DEF_LongMax, DEF_LongMax);
	e1.ToStringDocPagePos(bufScan);

	// Set new entry from the string buffer
	e2.Set(0, 0, 0);
	e2.FromStringDocPagePos(bufScan);

  CPPUNIT_ASSERT(e2.mUiEntDoc == DEF_IntMax);
  CPPUNIT_ASSERT(e2.mUlEntPge == DEF_LongMax);
  CPPUNIT_ASSERT(e2.mUlEntPos == DEF_LongMax);
  CPPUNIT_ASSERT(strcmp(bufScan, "65535-4294967295-4294967295") == 0);
}
//------------------------------------------------------------------------------

void EntryTest::testEntryOperator(void)
{
	Entry e1(1, 1, 1), e2;

	e2 = e1;
  CPPUNIT_ASSERT(e1 == e2);

	e2 = e1;
	e2.mUiEntDoc++;
  CPPUNIT_ASSERT(e1 < e2);

	e2 = e1;
	e2.mUlEntPge++;
  CPPUNIT_ASSERT(e1 < e2);

	e2 = e1;
	e2.mUlEntPos++;
  CPPUNIT_ASSERT(e1 < e2);
}
//------------------------------------------------------------------------------

void EntryTest::testEntrySet(void)
{
	Entry e;
	EntrySet set1, set2;

	e.Set(1, 0, 0);
	set1.insert(e);
	e.Set(111, 222, 333);
	set1.insert(e);
	e.Set(DEF_IntMax, DEF_LongMax, DEF_LongMax);
	set1.insert(e);

	// Binary storage into flat buffer
	size_t dataSize;
	BinaryBuffer buffer;
	size_t nb1 = EntrySetToBuf(buffer, dataSize, set1);
	size_t nb2 = BufToEntrySet(set2, buffer.getBuffer(), buffer.getOccupancy(), NULL);
	size_t nb3 = GetBufEntrySetCount(buffer.getBuffer());

	// Check the size of the sets
  CPPUNIT_ASSERT(nb1 == nb2);
  CPPUNIT_ASSERT(set1.size() == nb1);
  CPPUNIT_ASSERT(set2.size() == nb2);
  CPPUNIT_ASSERT(nb2 == nb3);

	// Check the content of the sets
	Entry e1, e2;
	EntrySet::iterator it1, it2;	// Set iterator
	for (it1 = set1.begin(), it2 = set2.begin(); it1 != set1.end(); ++it1, ++it2) {
		e1 = (*it1);
		e2 = (*it2);
		CPPUNIT_ASSERT(e1 == e2);
	}

	// Check the binary storage of an empty set
	EntrySet set;
	EntrySetToBuf(buffer, dataSize, set);
	size_t nb = BufToEntrySet(set, buffer.getBuffer(), buffer.getOccupancy(), NULL);
  CPPUNIT_ASSERT(nb == 0);
}
//------------------------------------------------------------------------------
