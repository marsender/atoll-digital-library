/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

CollectionTest.hpp

*******************************************************************************/

#ifndef __CollectionTest_HPP
#define	__CollectionTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class Collection;
}

//! Collection test class
class CollectionTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(CollectionTest);
  CPPUNIT_TEST(testDocMap);
  CPPUNIT_TEST(testDublinCoreDoc);
  CPPUNIT_TEST_SUITE_END();

public:
  CollectionTest();
  virtual ~CollectionTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testDocMap(void);
  void testDublinCoreDoc(void);

private:
	std::string mPath;
	Atoll::EngineEnv *mEngineEnv;
	Atoll::Collection *mCollection;

	// No need for copy and assignment
	CollectionTest(const CollectionTest &);
	CollectionTest &operator=(const CollectionTest &);
};
//------------------------------------------------------------------------------

#endif
