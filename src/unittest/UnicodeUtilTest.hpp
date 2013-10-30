/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnicodeUtilTest.hpp

*******************************************************************************/

#ifndef __UnicodeUtilTest_HPP
#define	__UnicodeUtilTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Unicode utilities test class
class UnicodeUtilTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(UnicodeUtilTest);
  CPPUNIT_TEST(testStringDecl);
  CPPUNIT_TEST(testFile2UnicodeString);
	CPPUNIT_TEST(testUnicodeString2String);
	CPPUNIT_TEST(testUnicodeStringStrip);
	CPPUNIT_TEST(testUnicodeStringGetContent);
	CPPUNIT_TEST(testUnicodeStringGetLine);
  CPPUNIT_TEST(testUnicodeStringClearCrLf);
  CPPUNIT_TEST(testUnicodeStringHasSpace);
  CPPUNIT_TEST(testUnicodeStringConversion);
	CPPUNIT_TEST(testUnicodeStringToLower);
	CPPUNIT_TEST(testUnicodeStringAddXmlEntities);
  CPPUNIT_TEST_SUITE_END();

public:
  UnicodeUtilTest();
  virtual ~UnicodeUtilTest();

	//! Setup called before each test
  void setUp();
	//! Shutdown called after each test
  void tearDown();

protected:
	std::string mPath;
	//! Test UChar string declaration
  void testStringDecl();
	//! Test read / write from file
  void testFile2UnicodeString();
	void testUnicodeString2String();
	void testUnicodeStringStrip();
	void testUnicodeStringGetContent();
	void testUnicodeStringGetLine();
	//! Clear the line feeds, trim and remove duplicate spaces in a string
	void testUnicodeStringClearCrLf();
	//! Indicate if a string contains spaces, tabulations or line feeds
	void testUnicodeStringHasSpace();
	//! Conversion between string and unicode string
	void testUnicodeStringConversion();
	//! Lower a string
	void testUnicodeStringToLower();
	//! Add a string with xml special chars replaced with predefined entities
	void testUnicodeStringAddXmlEntities();

private:
	// No need for copy and assignment
	UnicodeUtilTest(const UnicodeUtilTest &);
	UnicodeUtilTest &operator=(const UnicodeUtilTest &);
	//! Run the test on one file
  void runFile2UnicodeString(const char *inFile);
};
//------------------------------------------------------------------------------

#endif
