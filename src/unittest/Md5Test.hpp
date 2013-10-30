/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Md5Test.hpp

*******************************************************************************/

#ifndef __Md5Test_HPP
#define	__Md5Test_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Md5 test class
class Md5Test : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(Md5Test);
  CPPUNIT_TEST(testMain);
  CPPUNIT_TEST_SUITE_END();

public:
  Md5Test();
  virtual ~Md5Test();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Main tests
  void testMain(void);

private:
	std::string mPath;
	// No need for copy and assignment
	Md5Test(const Md5Test &);
	Md5Test &operator=(const Md5Test &);
};
//------------------------------------------------------------------------------

#endif
