/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerTest.hpp

*******************************************************************************/

#ifndef __RecordBreakerTest_HPP
#define	__RecordBreakerTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! RecordBreaker test class
class RecordBreakerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(RecordBreakerTest);
  CPPUNIT_TEST(testRecordBreaker);
  CPPUNIT_TEST(testRecordBreakerConfig);
  CPPUNIT_TEST_SUITE_END();

public:
  RecordBreakerTest();
  virtual ~RecordBreakerTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Test the record breaker
  void testRecordBreaker(void);
	//! Test the xml configuration of the record breaker
  void testRecordBreakerConfig(void);

private:
	std::string mPath;

	// No need for copy and assignment
	RecordBreakerTest(const RecordBreakerTest &);
	RecordBreakerTest &operator=(const RecordBreakerTest &);
};
//------------------------------------------------------------------------------

#endif
