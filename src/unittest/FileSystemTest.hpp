/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

FileSystemTest.hpp

*******************************************************************************/

#ifndef __FileSystemTest_HPP
#define	__FileSystemTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! FileSystem test class
class FileSystemTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(FileSystemTest);
  CPPUNIT_TEST(testCreateDirectory);
	CPPUNIT_TEST(testGetDirectoryFiles);
	CPPUNIT_TEST(testGetPath);
	CPPUNIT_TEST(testGetFileName);
	CPPUNIT_TEST(testGetExtension);
	CPPUNIT_TEST(testSetExtension);
	CPPUNIT_TEST(testMakeFullFileName);
  CPPUNIT_TEST_SUITE_END();

public:
  FileSystemTest();
  virtual ~FileSystemTest();

	//! Setup called before each test
  void setUp();
	//! Shutdown called after each test
  void tearDown();

protected:
	std::string mPath;
  void testCreateDirectory();
	void testGetDirectoryFiles();
	void testGetPath();
	void testGetFileName();
	void testGetExtension();
	void testSetExtension();
	void testMakeFullFileName();

private:
	// No need for copy and assignment
	FileSystemTest(const FileSystemTest &);
	FileSystemTest &operator=(const FileSystemTest &);
};
//------------------------------------------------------------------------------

#endif
