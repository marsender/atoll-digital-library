/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

CollectionTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "CollectionTest.hpp"
#include "UnitTestSuites.hpp"
#include "../collection/Collection.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../database/DbNdx.hpp"
#include "../engine/EngineEnv.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CollectionTest, Atoll::UnitTestSuites::Collection());

CollectionTest::CollectionTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


CollectionTest::~CollectionTest()
{
}
//------------------------------------------------------------------------------

void CollectionTest::setUp(void)
{
	mEngineEnv = new EngineEnv;
	if (!mEngineEnv->CreateDbEnv(Atoll::UnitTestSuites::GetDbHome())) {
		delete mEngineEnv;
		CPPUNIT_ASSERT_MESSAGE("Create Db env error", false);
	}
	mEngineEnv->SetColId(Atoll::UnitTestSuites::GetColId());

	// Create collection
	mCollection = new Collection(mEngineEnv->GetDbEnv(), mEngineEnv->GetColId());

	// Remove all documents and index
	mCollection->ClearCollection();
}
//------------------------------------------------------------------------------

void CollectionTest::tearDown(void)
{
	delete mCollection;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void CollectionTest::testDocMap(void)
{
	bool isLogError;
	bool isOk;
	bool xmlValidation = false;
	DocMeta docMeta, docMeta1, docMeta2;
	unsigned int docNum;
	unsigned int docNum1 = 1, docNum2 = 2;
	FieldMeta fieldMeta;
	eTypField typField = eTypFieldSearch;
	unsigned int fieldOrder = 5;
	UnicodeString fieldId = "text";
	UnicodeString fieldTitle = "Full text";
	UnicodeString docTitle = "The document title";

	std::string dcMetadata = mPath + "dublincore_test.xml"; // We could set a different file for each doc
	std::string indexerConfig = mPath + "indexer_config_test.xml";
	std::string recordBreakerConfig = mPath + "recordbreaker_config_test.xml";

	std::string docFileName1 = mPath + "docbook_4-4.xml";
	std::string docFileName2 = mPath + "docbook_5.xml";

	// Add a document
	docMeta1.mFileName = docFileName1;
	docMeta1.mXmlValidation = xmlValidation;
	docMeta1.mDcMetadata = dcMetadata; // Not mandatory
	docMeta1.mIndexerConfig = indexerConfig;
	docMeta1.mRecordBreakerConfig = recordBreakerConfig;
	docMeta1.mDocTitle = docTitle; // Not mandatory
	isOk = mCollection->AddDocument(docNum, docMeta1);
	CPPUNIT_ASSERT_MESSAGE("Add document error", isOk);
	CPPUNIT_ASSERT(docNum == docNum1);

	// Add another document
	docMeta2.mFileName = docFileName2;
	docMeta2.mXmlValidation = xmlValidation;
	docMeta1.mDcMetadata = dcMetadata; // Not mandatory
	docMeta2.mIndexerConfig = indexerConfig;
	docMeta2.mRecordBreakerConfig = recordBreakerConfig;
	docMeta2.mDocTitle = docTitle; // Not mandatory
	isOk = mCollection->AddDocument(docNum, docMeta2);
	CPPUNIT_ASSERT_MESSAGE("Add document error", isOk);
	CPPUNIT_ASSERT(docNum == docNum2);

	// Check if the first document metadata indexed flag is true
	docMeta.mDocNum = docNum1;
	isOk = mCollection->GetDocumentMeta(docMeta);
	CPPUNIT_ASSERT_MESSAGE("Get document meta error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Non indexed document error", docMeta.mIsIndexed);
	CPPUNIT_ASSERT_MESSAGE("Document without pages error", docMeta.mCountPge);

	// Add an already existing document
	isLogError = gLog.isInLogLevel(eTypLogError);
	gLog.removeLogLevel(eTypLogError); // Remove errors
	isOk = mCollection->AddDocument(docNum, docMeta1);
	if (isLogError)
		gLog.addLogLevel(eTypLogError); // Show errors
	CPPUNIT_ASSERT_MESSAGE("Add same document error", !isOk);

	// Check the number of documents
	CPPUNIT_ASSERT_MESSAGE("Document count", mCollection->GetDocumentCount() == 2);

	// Add a field
	fieldMeta.mTypField = typField;
	fieldMeta.mFieldOrder = fieldOrder;
	fieldMeta.SetFieldId(fieldId);
	fieldMeta.mFieldTitle = fieldTitle;
	isOk = mCollection->AddFieldMeta(fieldMeta);
	CPPUNIT_ASSERT_MESSAGE("Add field meta error", isOk);

	// Reopen the collection
	delete mCollection;
	mCollection = new Collection(mEngineEnv->GetDbEnv(), mEngineEnv->GetColId());

	// Check the number of documents
	CPPUNIT_ASSERT_MESSAGE("Document count", mCollection->GetDocumentCount() == 2);

	// Check if the documents still exist
	CPPUNIT_ASSERT(mCollection->DocumentExist(docNum1));
	CPPUNIT_ASSERT(mCollection->DocumentExist(docNum2));

	// Check a non existing document
	CPPUNIT_ASSERT_MESSAGE("Unknown document exist", !mCollection->DocumentExist(DEF_IntMax));

	// Remove the first document
	isOk = mCollection->RemoveDocument(docNum1);
	CPPUNIT_ASSERT_MESSAGE("Remove document error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Removed document still exist", !mCollection->DocumentExist(docNum1));

	// Remove the second document
	isOk = mCollection->RemoveDocument(docNum2);
	CPPUNIT_ASSERT_MESSAGE("Remove document error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Removed document still exist", !mCollection->DocumentExist(docNum2));

	// Remove twice
	isLogError = gLog.isInLogLevel(eTypLogError);
	gLog.removeLogLevel(eTypLogError); // Remove errors
	isOk = mCollection->RemoveDocument(docMeta1.mDocNum);
	if (isLogError)
		gLog.addLogLevel(eTypLogError); // Show errors
	CPPUNIT_ASSERT_MESSAGE("Remove document twice error", !isOk);

	// Check if the databases are empty after removing all the documents
	CPPUNIT_ASSERT_MESSAGE("Document count not empty", mCollection->GetDocumentCount() == 0);

	// Check if the field still exist
	fieldMeta.Clear();
	fieldMeta.mTypField = typField;
	fieldMeta.SetFieldId(fieldId);
	isOk = mCollection->GetFieldMeta(fieldMeta);
	CPPUNIT_ASSERT_MESSAGE("Get field meta error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Get field meta value error", fieldMeta.mTypField == typField
		&& fieldMeta.mFieldOrder == fieldOrder && fieldMeta.GetFieldId() == fieldId
		&& fieldMeta.mFieldTitle == fieldTitle);

	// Remove the field
	isOk = mCollection->RemoveFieldMeta(fieldMeta);
	CPPUNIT_ASSERT_MESSAGE("Remove field meta error", isOk);

	// Check a non existing field
	isOk = mCollection->GetFieldMeta(fieldMeta);
	CPPUNIT_ASSERT_MESSAGE("Removed field still exist", !isOk);

	unsigned long indexCount = mCollection->GetIndexCount();
	if (indexCount) {
		UnicodeStringToIntMap map;
		mCollection->GetIndexMap(map);
		UnicodeStringToIntMap::const_iterator it = map.begin();
		UnicodeStringToIntMap::const_iterator itEnd = map.end();
		for (; it != itEnd; ++it) {
			const UnicodeString &uStr = it->first;
			const unsigned int &value = it->second;
			std::string str = UnicodeString2String(uStr);
			gLog.log(eTypLogError, "Err > Index is not empty: %s [size=%u]", str.c_str(), value);
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Index count not empty", indexCount == 0);
}
//------------------------------------------------------------------------------

void CollectionTest::testDublinCoreDoc(void)
{
	bool isOk;
	bool xmlValidation = false;
	DocMeta docMeta, docMeta1;
	unsigned int docNum;
	unsigned int docNum1 = 1;
	UnicodeString docTitle = "The document title";

	std::string indexerConfig = mPath + "dublincore_indexer_config.xml";
	std::string recordBreakerConfig = mPath + "dublincore_recordbreaker_config.xml";

	std::string docFileName1 = mPath + "dublincore_test.xml";

	// Add a document
	docMeta1.mFileName = docFileName1;
	docMeta1.mXmlValidation = xmlValidation;
	docMeta1.mIndexerConfig = indexerConfig;
	docMeta1.mRecordBreakerConfig = recordBreakerConfig;
	docMeta1.mDocTitle = docTitle; // Not mandatory
	isOk = mCollection->AddDocument(docNum, docMeta1);
	CPPUNIT_ASSERT_MESSAGE("Add document error", isOk);
	CPPUNIT_ASSERT(docNum == docNum1);

	// Check if the first document metadata indexed flag is true
	docMeta.mDocNum = docNum1;
	isOk = mCollection->GetDocumentMeta(docMeta);
	CPPUNIT_ASSERT_MESSAGE("Get document meta error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Non indexed document error", docMeta.mIsIndexed);
	CPPUNIT_ASSERT_MESSAGE("Document without pages error", docMeta.mCountPge);

	unsigned long indexCount = mCollection->GetIndexCount();
	if (indexCount) {
		UnicodeStringToIntMap map;
		mCollection->GetIndexMap(map);
		UnicodeStringToIntMap::const_iterator it = map.begin();
		UnicodeStringToIntMap::const_iterator itEnd = map.end();
		for (; it != itEnd; ++it) {
			const UnicodeString &uStr = it->first;
			//const unsigned int &value = it->second;
			std::string str = UnicodeString2String(uStr);
			//gLog.log(eTypLogError, "Err > Index is not empty: %s [size=%u]", str.c_str(), value);
			DbNdx *dbNdx = mCollection->GetIndexDatabase(str);
			//UnicodeStringVector vector;
			//dbNdx->GetWordList(vector);
			//CPPUNIT_ASSERT(vector.size() == count);
			// Get the index content (words and occurrences)
			WordIntVector wordIntVector;
			unsigned long min = 0, nb = 10;
			isOk = dbNdx->GetWordIntList(wordIntVector, min, nb);
			size_t sSize = wordIntVector.size();
			CPPUNIT_ASSERT_MESSAGE("Count error", sSize >= 1);
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Index count is empty", indexCount != 0);
}
//------------------------------------------------------------------------------
