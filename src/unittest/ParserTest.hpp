/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ParserTest.hpp

*******************************************************************************/

#ifndef __ParserTest_HPP
#define	__ParserTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class Parser;
}

//! Xerces parser test class
class ParserTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ParserTest);
  CPPUNIT_TEST(testXercesStringLocalForm);
  CPPUNIT_TEST(testXercesStringUChar);
	CPPUNIT_TEST(testXercesParseFile);
	CPPUNIT_TEST(testXercesParseString);
  CPPUNIT_TEST_SUITE_END();

public:
  ParserTest();
  virtual ~ParserTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testXercesStringLocalForm(void);
  void testXercesStringUChar(void);
	void testXercesParseFile(void);
	void testXercesParseString(void);

private:
	std::string mPath;
	Atoll::Parser *mParser;

	// No need for copy and assignment
	ParserTest(const ParserTest &);
	ParserTest &operator=(const ParserTest &);
};
//------------------------------------------------------------------------------

#endif
