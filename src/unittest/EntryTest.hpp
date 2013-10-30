/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EntryTest.hpp

*******************************************************************************/

#ifndef __EntryTest_HPP
#define	__EntryTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Entry test class
class EntryTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(EntryTest);
  CPPUNIT_TEST(testEmpty);
  CPPUNIT_TEST(testDppMax);
  CPPUNIT_TEST(testBinaryStorage);
  CPPUNIT_TEST(testStringStorage);
  CPPUNIT_TEST(testEntryOperator);
  CPPUNIT_TEST(testEntrySet);
  CPPUNIT_TEST_SUITE_END();

public:
  EntryTest();
  virtual ~EntryTest();

  void setUp(void);
  void tearDown(void);

protected:
	//! Test the empty value
  void testEmpty(void);
	//! Test the max values
  void testDppMax(void);
  void testBinaryStorage(void);
  void testStringStorage(void);
  void testEntryOperator(void);
  void testEntrySet(void);

private:
	// No need for copy and assignment
	EntryTest(const EntryTest &);
	EntryTest &operator=(const EntryTest &);
};
//------------------------------------------------------------------------------

#endif
