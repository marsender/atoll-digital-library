/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

QueryParserTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "QueryParserTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../lucene/Lucene.hpp"
#include "../lucene/analysis/StandardAnalyzer.hpp"
#include "../lucene/queryparser/QueryParser.hpp"
#include "../lucene/search/Query.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace std;
using namespace Common;
using namespace Lucene::analysis;
using namespace Lucene::queryParser;
using namespace Lucene::search;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(QueryParserTest, Atoll::UnitTestSuites::Query());

struct TestQueries {
	const char *query;
	const char *expect;
};

static const TestQueries cTestQueries[] = {
	{ _T("term"), _T("term") },
	{ _T("term term term"), _T("term term term") },
	{ _T("a AND b"), _T("+a +b") },
	{ _T("(a AND b)"), _T("+a +b") },
	{ _T("c OR (a AND b)"), _T("c (+a +b)") },
	{ _T("a AND NOT b"), _T("+a -b") },
	{ _T("a AND -b"), _T("+a -b") },
	{ _T("a AND !b"), _T("+a -b") },
	{ _T("a && b"), _T("+a +b") },
	{ _T("a && ! b"), _T("+a -b") },
	{ _T("a OR b"), _T("a b") },
	{ _T("a || b"), _T("a b") },
	{ _T("a OR !b"), _T("a -b") },
	{ _T("a OR ! b"), _T("a -b") },
	{ _T("a OR -b"), _T("a -b") },
	{ _T("+term -term term"), _T("+term -term term") },
	{ _T("foo:term AND field:anotherTerm"), _T("+foo:term +anotherTerm") },
	{ _T("can't"), _T("\"can t\"") }, // Depends on ReadApostrophe
	{ _T("\"hello there\""), _T("\"hello there\"") },
	{ _T("term AND \"phrase phrase\""), _T("+term +\"phrase phrase\"") },
	{ _T("germ term^2.0"), _T("germ term^2.0") },
	{ _T("term^2.0"), _T("term^2.0") },
	{ _T("term^2"), _T("term^2.0") },
	{ _T("term^2.3"), _T("term^2.3") },
	{ _T("\"germ term\"^2.0"), _T("\"germ term\"^2.0") },
	{ _T("\"germ term\"^2.02"), _T("\"germ term\"^2.0") },
	{ _T("\"term germ\"^2"), _T("\"term germ\"^2.0") },
	{ _T("(foo OR bar) AND (baz OR boo)"), _T("+(foo bar) +(baz boo)") },
	{ _T("((a OR b) AND NOT c) OR d"), _T("(+(a b) -c) d") },
	{ _T("+(apple \"steve jobs\") -(foo bar baz)"), _T("+(apple \"steve jobs\") -(foo bar baz)") },
	{ _T("+title:(dog OR cat) -author:\"bob dole\""), _T("+(title:dog title:cat) -author:\"bob dole\"") },
	// Slop
	{ _T("\"term germ\"~2"), _T("\"term germ\"~2") },
	{ _T("\"term germ\"~2 flork"), _T("\"term germ\"~2 flork") },
	{ _T("\"term\"~2"), _T("term") },
	{ _T("\" \"~2 germ"), _T("germ") },
	{ _T("\"term germ\"~2^2"), _T("\"term germ\"~2^2.0") },
	// Numbers
	{ _T("3"), _T("3") },
	{ _T("term 1.0 1 2"), _T("term 1.0 1 2") },
	{ _T("term term1 term2"), _T("term term1 term2") },
	// Wildcard: QueryParserBase::GetWildcardQuery
	{ _T("term*"), _T("term*") },
	{ _T("term*^2"), _T("term*^2.0") },
	{ _T("term*germ"), _T("term*germ") },
	{ _T("term*germ^3"), _T("term*germ^3.0") },
	// Fuzzy: QueryParserBase::GetFuzzyQuery
	{ _T("term~"), _T("term~0.5") },
	{ _T("term~^2"), _T("term^2.0~0.5") },
	{ _T("term^2~"), _T("term^2.0~0.5") },
	// Range: QueryParserBase::GetRangeQuery
	{ _T("[ a z]"), _T("[a TO z]") },
	{ _T("[ a z ]"), _T("[a TO z]") },
	{ _T("{ a z}"), _T("{a TO z}") },
	{ _T("{ a z }"), _T("{a TO z}") },
	{ _T("{ a z }^2.0"), _T("{a TO z}^2.0") },
	{ _T("[ a z] OR bar"), _T("[a TO z] bar") },
	{ _T("[ a z] AND bar"), _T("+[a TO z] +bar") },
	{ _T("( bar blar { a z}) "), _T("bar blar {a TO z}") },
	{ _T("gack ( bar blar { a z}) "), _T("gack (bar blar {a TO z})") },
	{ NULL, NULL }
};

QueryParserTest::QueryParserTest()
{
}
//------------------------------------------------------------------------------


QueryParserTest::~QueryParserTest()
{
}
//------------------------------------------------------------------------------

void QueryParserTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void QueryParserTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

Query* QueryParserTest::GetQuery(const char *inStrQuery, Lucene::analysis::Analyzer *inAnalyzer)
{
	Query *q;
	UnicodeString query(inStrQuery);

	q = GetQuery(query.getTerminatedBuffer(), inAnalyzer);

	return q;
}
//------------------------------------------------------------------------------

Query* QueryParserTest::GetQuery(const UChar *inStrQuery, Lucene::analysis::Analyzer *inAnalyzer)
{
	Query *q;

	try {
		// Init the analyzer: WhitespaceAnalyzer StandardAnalyzer SimpleAnalyzer
		Analyzer *analyzer = inAnalyzer ? inAnalyzer : new StandardAnalyzer();

		UnicodeString field("field");
		QueryParser qp(field.getTerminatedBuffer(), analyzer);
		q = qp.parse(inStrQuery);

		if (inAnalyzer == NULL)
			delete analyzer;
	}
	catch (AppException &/*e*/) {
		//CuFail(tc, _T("/%s/ threw an error: %s - "), inStrQuery, e.twhat());
		q = NULL;
	}
	catch (...) {
		//CuFail(tc, _T("/%s/ threw an error.\n"), inStrQuery);
		q = NULL;
	}

	return q;
}
//------------------------------------------------------------------------------

bool QueryParserTest::AssertQueryEquals(const char* inStrQuery, Analyzer *inAnalyzer, const char* inStrExpect)
{
	bool isOk = true;

	try {
		std::string strYield;
		UnicodeString expect;
		expect = inStrExpect;
		Query *q = GetQuery(inStrQuery, inAnalyzer);
		if (q == NULL) {
			strYield = "NULL";
			isOk = false;
		}
		else {
			UnicodeString field("field");
			const UChar* s = q->toString(field.getTerminatedBuffer());
			if (u_strcmp(s, expect.getTerminatedBuffer()) != 0) {
				strYield = UnicodeString2String(s);
				isOk = false;
			}
			delete [] s;
		}
		if (!isOk) {
			mMessage += "AssertQueryEquals failed query[";
			mMessage += inStrQuery;
			mMessage += "] yield[";
			mMessage += strYield;
			mMessage += "] expected[";
			mMessage += inStrExpect;
			mMessage += "]";
			//CuFail(tc, _T("FAILED Query /%s/ yielded /%s/, expecting /%s/\n"), inStrQuery, s, result);
		}
		if (q)
			delete q;
	}
	catch (const AppException &e) {
		mMessage += e.what();
	}
	catch (std::exception &e) {
		mMessage += e.what();
	}
	catch (...) {
		mMessage += "Unknown exception";
	}

	return isOk;
}
//------------------------------------------------------------------------------

void QueryParserTest::testLuceneQueryInstanceOf(void)
{
	Query *q;
	UnicodeString instance;
	Analyzer *analyzer = NULL;

	instance = "TermQuery";
	q = GetQuery(_T("hello"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of TermQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;

	instance = "BooleanQuery";
	q = GetQuery(_T("a AND b"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of BooleanQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;

	instance = "PhraseQuery";
	q = GetQuery(_T("\"hello there\""), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of PhraseQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;

	instance = "RangeQuery";
	q = GetQuery(_T("[ a z]"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of RangeQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;
	q = GetQuery(_T("[a TO z]"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of RangeQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;
	q = GetQuery(_T("{a TO z}"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of RangeQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;

	instance = "PrefixQuery";
	q = GetQuery(_T("term*"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of PrefixQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;
	q = GetQuery(_T("term*^2"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of PrefixQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;

	instance = "WildcardQuery";
	q = GetQuery(_T("term*germ"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of WildcardQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;

	instance = "FuzzyQuery";
	q = GetQuery(_T("term~"), analyzer);
	CPPUNIT_ASSERT_MESSAGE("Instance of FuzzyQuery", q && q->instanceOf(instance.getTerminatedBuffer()));
	delete q;
}
//------------------------------------------------------------------------------

void QueryParserTest::testLuceneQueryParser(void)
{
	bool isOk;
	//const UChar *s;
	//UnicodeString uStr;
	//StandardAnalyzer a;

	/*
	Char2UnicodeString(uStr, "t\xc3\xbcrm term term");
	s = uStr.getTerminatedBuffer();
	isOk = AssertQueryEquals(s, NULL, s);
	CPPUNIT_ASSERT_MESSAGE("SimpleAnalyzer: t\xc3\xbcrm term term", isOk);
	isOk = AssertQueryEquals(s, &a, s);
	CPPUNIT_ASSERT_MESSAGE("StandardAnalyzer: t\xc3\xbcrm term term", isOk);

	Char2UnicodeString(uStr, "\xc3\xbcmlaut");
	s = uStr.getTerminatedBuffer();
	isOk = AssertQueryEquals(s, NULL, s);
	CPPUNIT_ASSERT_MESSAGE("SimpleAnalyzer: \xc3\xbcmlaut", isOk);
	isOk = AssertQueryEquals(s, &a, s);
	CPPUNIT_ASSERT_MESSAGE("StandardAnalyzer: \xc3\xbcmlaut", isOk);
	*/

	for (const TestQueries *p = cTestQueries; p->query; ++p) {
		mMessage = "";
		isOk = AssertQueryEquals(p->query, NULL, p->expect);
		if (!isOk) {
			char buf[255];
			sprintf(buf, "TestQueries indice = %d: ", (int)(p - cTestQueries + 1));
			std::string testName(buf);
			CPPUNIT_ASSERT_MESSAGE(testName + mMessage, isOk);
		}
	}
}
//------------------------------------------------------------------------------

void QueryParserTest::testLuceneWhitespaceAnalyzer(void)
{
	bool isOk;
	std::string testName("WhitespaceAnalyzer: ");
	WhitespaceAnalyzer analyzer;

	isOk = AssertQueryEquals(_T("a&b"), &analyzer, _T("a&b"));
	CPPUNIT_ASSERT_MESSAGE(testName + mMessage, isOk);

	isOk = AssertQueryEquals(_T("a&&b"), &analyzer, _T("a&&b"));
	CPPUNIT_ASSERT_MESSAGE(testName + mMessage, isOk);

	isOk = AssertQueryEquals(_T(".NET"), &analyzer, _T(".NET"));
	CPPUNIT_ASSERT_MESSAGE(testName + mMessage, isOk);
}
//------------------------------------------------------------------------------

void QueryParserTest::testLuceneStandardAnalyzer(void)
{
	bool isOk;
	std::string testName("StandardAnalyzer: ");
	StandardAnalyzer analyzer;

	isOk = AssertQueryEquals(_T("3"), &analyzer, _T("3"));
	CPPUNIT_ASSERT_MESSAGE(testName + mMessage, isOk);

	isOk = AssertQueryEquals(_T("term 1.0 1 2"), &analyzer, _T("term 1.0 1 2"));
	CPPUNIT_ASSERT_MESSAGE(testName + mMessage, isOk);

	isOk = AssertQueryEquals(_T("term term1 term2"), &analyzer, _T("term term1 term2"));
	CPPUNIT_ASSERT_MESSAGE(testName + mMessage, isOk);
}
//------------------------------------------------------------------------------
