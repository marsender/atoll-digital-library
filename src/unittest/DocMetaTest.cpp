/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocMetaTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DocMetaTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../util/DocMeta.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DocMetaTest, Atoll::UnitTestSuites::Util());

DocMetaTest::DocMetaTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


DocMetaTest::~DocMetaTest()
{
}
//------------------------------------------------------------------------------

void DocMetaTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void DocMetaTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void DocMetaTest::testSerialise(void)
{
	unsigned int num = DEF_IntMax;
	unsigned long countPge = DEF_LongMax;
	bool xmlValidation = true;
	bool isIndexed = true;
	UnicodeString title("Document title");
	UnicodeString uuid("urn:uuid:6A5CF297-6BF9-1014-8A5B-C107EBBCBEE4");
	UnicodeString xsltCode("docbook-xsl-ns");
	std::string source("./test.epub");
	std::string fileName("./test.xml");

	std::string dcMetadata = mPath + "dublincore_test.xml";
	std::string indexerConfig = mPath + "indexer_config_test.xml";
	std::string recordBreakerConfig = mPath + "recordbreaker_config_test.xml";

	// Init record
	DocMeta dm;
	dm.mDocNum = num;
	dm.mSource = source;
	dm.mCountPge = countPge;
	dm.mXmlValidation = xmlValidation;
	dm.mIsIndexed = isIndexed;
	dm.mFileName = fileName;
	dm.mDocTitle = title;
	dm.mUuid = uuid;
	dm.mXsltCode = xsltCode;
	dm.mDcMetadata = dcMetadata;
	dm.mIndexerConfig = indexerConfig;
	dm.mRecordBreakerConfig = recordBreakerConfig;

	BinaryBuffer binaryBuffer;
	dm.ToBinaryBuffer(binaryBuffer);
	dm.Clear();
	dm.FromBinaryBuffer(binaryBuffer);

	CPPUNIT_ASSERT(dm.mDocNum == num);
	CPPUNIT_ASSERT(dm.mSource == source);
	CPPUNIT_ASSERT(dm.mCountPge == countPge);
	CPPUNIT_ASSERT(dm.mXmlValidation == xmlValidation);
	CPPUNIT_ASSERT(dm.mIsIndexed == isIndexed);
	CPPUNIT_ASSERT(dm.mDocTitle == title);
	CPPUNIT_ASSERT(dm.mUuid == uuid);
	CPPUNIT_ASSERT(dm.mXsltCode == xsltCode);
	CPPUNIT_ASSERT(dm.mFileName == fileName);
	CPPUNIT_ASSERT(dm.mDcMetadata == dcMetadata);
	CPPUNIT_ASSERT(dm.mIndexerConfig == indexerConfig);
	CPPUNIT_ASSERT(dm.mRecordBreakerConfig == recordBreakerConfig);
}
//------------------------------------------------------------------------------
