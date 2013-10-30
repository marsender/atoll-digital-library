/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerHandlerTest.hpp

*******************************************************************************/

#ifndef __AdornerHandlerTest_HPP
#define	__AdornerHandlerTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class Parser;
class AdornerHandler;
}

//! Xerces AdornerHandler test class
class AdornerHandlerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(AdornerHandlerTest);
  CPPUNIT_TEST(testAdornerHandler);
  CPPUNIT_TEST(testAdornerHandlerOutputString);
  CPPUNIT_TEST(testAdornerHandlerRecordBreaker);
  CPPUNIT_TEST_SUITE_END();

public:
  AdornerHandlerTest();
  virtual ~AdornerHandlerTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testAdornerHandler(void);
  void testAdornerHandlerOutputString(void);
	void testAdornerHandlerRecordBreaker(void);

private:
	std::string mPath;
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::Parser *mParser;
	Atoll::AdornerHandler *mAdornerHandler;

	// No need for copy and assignment
	AdornerHandlerTest(const AdornerHandlerTest &);
	AdornerHandlerTest &operator=(const AdornerHandlerTest &);
};
//------------------------------------------------------------------------------

#endif
