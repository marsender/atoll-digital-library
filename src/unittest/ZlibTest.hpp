/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ZlibTest.hpp

*******************************************************************************/

#ifndef __ZlibTest_HPP
#define	__ZlibTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Zlib test class
class ZlibTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ZlibTest);
  CPPUNIT_TEST(testCompress);
  CPPUNIT_TEST(testZlibCompress);
  CPPUNIT_TEST(testZlibCompressBinary);
  CPPUNIT_TEST(testZlibCompressFileContent);
  CPPUNIT_TEST_SUITE_END();

public:
  ZlibTest();
  virtual ~ZlibTest();

	//! Setup called before each test
  void setUp();
	//! Shutdown called after each test
  void tearDown();

protected:
	std::string mPath;
  void testCompress();
  void testZlibCompress();
  void testZlibCompressBinary();
  void testZlibCompressFileContent();

private:
	// No need for copy and assignment
	ZlibTest(const ZlibTest &);
	ZlibTest &operator=(const ZlibTest &);

  void ZlibCompressFileContent(const char *inConverterName);
};
//------------------------------------------------------------------------------

#endif
