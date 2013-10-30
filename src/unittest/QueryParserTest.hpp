/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

QueryParserTest.hpp

*******************************************************************************/

#ifndef __QueryParserTest_HPP
#define	__QueryParserTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Lucene { namespace analysis {
	class Analyzer;
}}
namespace Lucene { namespace search {
	class Query;
}}

//! Lucene QueryParser test class
class QueryParserTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(QueryParserTest);
  CPPUNIT_TEST(testLuceneQueryInstanceOf);
  CPPUNIT_TEST(testLuceneQueryParser);
  CPPUNIT_TEST(testLuceneWhitespaceAnalyzer);
  CPPUNIT_TEST(testLuceneStandardAnalyzer);
  CPPUNIT_TEST_SUITE_END();

public:
  QueryParserTest();
  virtual ~QueryParserTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Query instance test
  void testLuceneQueryInstanceOf(void);
	//! Query parser test
  void testLuceneQueryParser(void);
	//! Whitespace analyzer test
  void testLuceneWhitespaceAnalyzer(void);
	//! Standard analyzer test
  void testLuceneStandardAnalyzer(void);

private:
	std::string mMessage;

	// No need for copy and assignment
	QueryParserTest(const QueryParserTest &);
	QueryParserTest &operator=(const QueryParserTest &);

	Lucene::search::Query *GetQuery(const char *inStrQuery, Lucene::analysis::Analyzer *inAnalyzer);
	Lucene::search::Query *GetQuery(const UChar *inStrQuery, Lucene::analysis::Analyzer *inAnalyzer);
	bool AssertQueryEquals(const char* inStrQuery, Lucene::analysis::Analyzer *inAnalyzer, const char* inStrExpect);
};
//------------------------------------------------------------------------------

#endif
