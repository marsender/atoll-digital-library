/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

FileSystemTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "FileSystemTest.hpp"
#include "UnitTestSuites.hpp"
//------------------------------------------------------------------------------

using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FileSystemTest, Atoll::UnitTestSuites::Common());

FileSystemTest::FileSystemTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


FileSystemTest::~FileSystemTest()
{
}
//------------------------------------------------------------------------------

void FileSystemTest::setUp()
{
}
//------------------------------------------------------------------------------

void FileSystemTest::tearDown()
{
}
//------------------------------------------------------------------------------

void FileSystemTest::testCreateDirectory()
{
	bool isOk;
	std::string path, mess;

	path = mPath + "dummy_testCreateDirectory";

	// The directory must not exist
	if (FileSystem::DirectoryExists(path)) {
		mess = "Directory already exists: " + path;
		CPPUNIT_ASSERT_MESSAGE(mess, false);
	}

	isOk = FileSystem::CreateNewDirectory(path);
	if (!isOk) {
		mess = "Directory cannot be created: " + path;
		CPPUNIT_ASSERT_MESSAGE(mess, false);
	}

	isOk = FileSystem::RemoveEmptyDirectory(path);
	if (!isOk) {
		mess = "Directory cannot be deleted: " + path;
		CPPUNIT_ASSERT_MESSAGE(mess, false);
	}
}
//------------------------------------------------------------------------------

void FileSystemTest::testGetDirectoryFiles()
{
	FileSystem::FileSet fileSet;
	std::string path = mPath;
	char msk[] = "*.Xml";
	bool isCase = false;
	bool isRecurs = true;
	bool wantDir = false;
	
	FileSystem::GetDirectoryFiles(fileSet, path.c_str(), msk, isCase, isRecurs, wantDir);
	CPPUNIT_ASSERT(fileSet.size() > 0);
}
//------------------------------------------------------------------------------

void FileSystemTest::testGetPath()
{
	std::string path, fileName, fullName, res;

	path = "./data_unittest";
	fileName = "test.txt";
	fullName = path;
	fullName += DEF_DirectorySeparator;
	fullName += fileName;
	res = FileSystem::GetPath(fullName);
	CPPUNIT_ASSERT(res == path);

	// Test without directory separator
	fullName = "data_unittest";
	res = FileSystem::GetPath(fullName);
	CPPUNIT_ASSERT(res.empty());

	// Test with an empty string
	fullName.erase();
	res = FileSystem::GetPath(fullName);
	CPPUNIT_ASSERT(res.empty());
}
//------------------------------------------------------------------------------

void FileSystemTest::testGetFileName()
{
	std::string path, fileName, fullName, res;

	path = "./data_unittest";
	fileName = "test.txt";
	fullName = path;
	fullName += DEF_DirectorySeparator;
	fullName += fileName;
	res = FileSystem::GetFileName(fullName);
	CPPUNIT_ASSERT(res == fileName);

	// Test without file name
	fullName = "./data_unittest/";
	res = FileSystem::GetFileName(fullName);
	CPPUNIT_ASSERT(res.empty());

	// Test with an empty string
	fullName.erase();
	res = FileSystem::GetFileName(fullName);
	CPPUNIT_ASSERT(res.empty());
}
//------------------------------------------------------------------------------

void FileSystemTest::testGetExtension()
{
	std::string ext, fullName, res;

	ext = "txt";
	fullName = "./data_unittest/test." + ext;
	res = FileSystem::GetExtension(fullName);
	CPPUNIT_ASSERT(res == ext);

	// Test without extension
	fullName = "test";
	res = FileSystem::GetExtension(fullName);
	CPPUNIT_ASSERT(res.empty());

	// Test without extension but with a dot before the filename
	fullName = "./data_unittest/test";
	res = FileSystem::GetExtension(fullName);
	CPPUNIT_ASSERT(res.empty());

	// Test with an empty string
	fullName.erase();
	res = FileSystem::GetExtension(fullName);
	CPPUNIT_ASSERT(res.empty());
}
//------------------------------------------------------------------------------

void FileSystemTest::testSetExtension()
{
	std::string ext, fullName, res;

	fullName = "./data_unittest/test.txt";

	ext = "txt";
	res = FileSystem::SetExtension(fullName, ext);
	CPPUNIT_ASSERT(res == fullName);

	ext = ".txt";
	res = FileSystem::SetExtension(fullName, ext);
	CPPUNIT_ASSERT(res == fullName);

	ext = "tst";
	res = FileSystem::SetExtension(fullName, ext);
	CPPUNIT_ASSERT(FileSystem::GetExtension(res) == ext);
}
//------------------------------------------------------------------------------

void FileSystemTest::testMakeFullFileName()
{
	std::string path, fileName, ext, res;

	path = "./data_unittest";
	fileName = "test";
	ext = "txt";
	res = FileSystem::MakeFullFileName(path, fileName, ext);
	CPPUNIT_ASSERT(res == "./data_unittest/test.txt");

	path = "./data_unittest/";
	fileName = "test";
	ext = "txt";
	res = FileSystem::MakeFullFileName(path, fileName, ext);
	CPPUNIT_ASSERT(res == "./data_unittest/test.txt");

	// Test without path
	path = "";
	fileName = "test";
	ext = "txt";
	res = FileSystem::MakeFullFileName(path, fileName, ext);
	CPPUNIT_ASSERT(res == "test.txt");

	// Test without ext
	path = "./data_unittest";
	fileName = "test";
	ext = "";
	res = FileSystem::MakeFullFileName(path, fileName, ext);
	CPPUNIT_ASSERT(res == "./data_unittest/test");
}
//------------------------------------------------------------------------------
