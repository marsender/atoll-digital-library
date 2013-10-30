/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "RecordBreakerTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/XercesNode.hpp"
#include "../recordbreaker/RecordBreaker.hpp"
#include "../recordbreaker/RecordBreakerConfig.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(RecordBreakerTest, Atoll::UnitTestSuites::RecordBreaker());

RecordBreakerTest::RecordBreakerTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


RecordBreakerTest::~RecordBreakerTest()
{
}
//------------------------------------------------------------------------------

void RecordBreakerTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void RecordBreakerTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void RecordBreakerTest::testRecordBreaker(void)
{
	std::string configFileName = mPath + "recordbreaker_config_test.xml";

	// Load the config
	RecordBreakerConfig config;
	config.LoadFromFile(configFileName);

	XercesNodeVector xercesNodeVector;

	RecordBreaker *recordBreaker = new RecordBreaker(&config, &xercesNodeVector);

	// Init record breaker
	recordBreaker->StartDocument();
	recordBreaker->mPge = 1; // As the open breaker waits for the first page

	std::string elem;
	std::string attr;
	UnicodeString value;
	Common::StringToUnicodeStringMap attrMap;
	eTypBreaker typBreaker;

	// Test open breaker
	elem = "pg";
	typBreaker = recordBreaker->ComputeNodeTypBreaker(elem, attrMap, true);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerOpen);

	// Test close breaker
	elem = "head";
	typBreaker = recordBreaker->ComputeNodeTypBreaker(elem, attrMap, false);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerClose);

	// Test store breaker
	elem = "title";
	typBreaker = recordBreaker->ComputeNodeTypBreaker(elem, attrMap, true);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerNone);

	// Test unknown breaker
	elem = "unused";
	typBreaker = recordBreaker->ComputeNodeTypBreaker(elem, attrMap, true);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerNone);

	// Test attribute + value
	attrMap.clear();
	elem = "node"; // Unused by the config test
	attr = "breaker";
	value = "page";
	attrMap[attr] = value;
	typBreaker = recordBreaker->ComputeNodeTypBreaker(elem, attrMap, true);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerOpen);

	// Test attribute only
	attrMap.clear();
	elem = "node"; // Unused by the config test
	attr = "pg";
	value = "5"; // Unused by the config test
	attrMap[attr] = value;
	typBreaker = recordBreaker->ComputeNodeTypBreaker(elem, attrMap, true);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerOpen);

	// Test processing instruction
	const UChar cPageBreak[] = { 'p', 'a', 'g', 'e', 'b', 'r', 'e', 'a', 'k', 0 };
	typBreaker = recordBreaker->ComputeProcessingInstruction(cPageBreak);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerOpen);

	// Test other processing instruction
	const UChar cUnused[] = { 'u', 'n', 'u', 's', 'e', 'd', 0 };
	typBreaker = recordBreaker->ComputeProcessingInstruction(cUnused);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerNone);

	// End record breaker
	recordBreaker->EndDocument();

	delete recordBreaker;
}
//------------------------------------------------------------------------------

void RecordBreakerTest::testRecordBreakerConfig(void)
{
	std::string configFileName = mPath + "recordbreaker_config_test.xml";

	RecordBreakerConfig config;
	config.LoadFromFile(configFileName);

	std::string elem;
	std::string attr;
	UnicodeString value;
	Common::StringToUnicodeStringMap attrMap;
	eTypBreaker typBreaker;

	// Test open breaker
	elem = "pg";
	typBreaker = config.GetNodeTypBreaker(elem, attrMap);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerOpen);

	// Test close breaker
	elem = "head";
	typBreaker = config.GetNodeTypBreaker(elem, attrMap);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerClose);

	// Test store breaker
	elem = "title";
	typBreaker = config.GetNodeTypBreaker(elem, attrMap);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerStore);

	// Test unknown breaker
	elem = "unused";
	typBreaker = config.GetNodeTypBreaker(elem, attrMap);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerNone);

	// Test attribute + value
	attrMap.clear();
	elem = "node"; // Unused by the config test
	attr = "breaker";
	value = "page";
	attrMap[attr] = value;
	typBreaker = config.GetNodeTypBreaker(elem, attrMap);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerOpen);

	// Test attribute only
	attrMap.clear();
	elem = "node"; // Unused by the config test
	attr = "pg";
	value = "5"; // Unused by the config test
	attrMap[attr] = value;
	typBreaker = config.GetNodeTypBreaker(elem, attrMap);
	CPPUNIT_ASSERT(typBreaker == eTypBreakerOpen);
}
//------------------------------------------------------------------------------
