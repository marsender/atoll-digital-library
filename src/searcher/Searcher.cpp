/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Searcher.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Searcher.hpp"
#include "QueryResolver.hpp"
#include "../collection/Collection.hpp"
#include "../database/DbNdx.hpp"
#include "../lucene/Lucene.hpp"
//#include "../lucene/analysis/Analyzers.hpp"
#include "../lucene/analysis/StandardAnalyzer.hpp"
#include "../lucene/index/SegmentTermEnum.hpp"
#include "../lucene/index/Term.hpp"
#include "../lucene/queryparser/QueryParser.hpp"
#include "../lucene/search/Query.hpp"
//#include "../lucene/search/Hits.hpp"
//#include "../lucene/search/TopDocs.hpp"
#include "../lucene/index/Terms.hpp"
#include "../common/UnicodeUtil.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

static const UChar cStrFieldPattern[] = { 0x00B7, 'f', 0x00B7, 0 }; // ·f·
static const UChar cStrWordPattern[] = { 0x00B7, 'w', 0x00B7, 0 }; // ·w·

Searcher::Searcher(Collection *inCollection)
	:mCollection(inCollection)
{
	mQuery = NULL;
	mQueryResolver = NULL;
	mEntrySet = new EntrySet;
}
//------------------------------------------------------------------------------

Searcher::~Searcher()
{
	AppAssert(mQueryResolver == NULL);

	_CLDELETE(mQuery);
	delete mEntrySet;
}
//------------------------------------------------------------------------------

DbNdx *Searcher::GetFieldDatabase(const UChar *inStrField) const
{
	std::string dbName;
	UChar2String(dbName, inStrField);

	// Get the database for this field, or null if the database doesn't exist
	DbNdx *dbNdx = mCollection->GetIndexDatabase(dbName);
	if (!dbNdx)
		gLog.log(eTypLogError, "Err > GetFieldDatabase: Unknown index: %s", dbName.c_str());

	return dbNdx;
}
//------------------------------------------------------------------------------

void Searcher::SetQuery(const UChar *inStrField, const UChar *inStrQuery, int32_t inLength /*= -1*/)
{
	Lucene::search::Query *query;
	int32_t len = inLength == -1 ? _tcslen(inStrQuery) : inLength;

	// Set the field pattern to the default word pattern
	UChar *p = _tcsstr(inStrField, cStrFieldPattern);
	if (p) {
		*(p + 1) = *(cStrWordPattern + 1);
	}

	try {
		// Init the analyzer: WhitespaceAnalyzer StandardAnalyzer SimpleAnalyzer
		Lucene::analysis::StandardAnalyzer analyzer;
		Lucene::queryParser::QueryParser queryParser(inStrField, &analyzer);
		query = queryParser.parse(inStrQuery, len);
	}
	catch (AppException &e) {
		query = NULL;
		std::string str;
		ConvertUnicodeString2String(str, inStrQuery, len);
		gLog.log(eTypLogError, "Err > Searcher SetQuery: Query -%s- threw an error: %s", str.c_str(), e.what());
	}
	catch (...) {
		query = NULL;
		std::string str;
		ConvertUnicodeString2String(str, inStrQuery, len);
		gLog.log(eTypLogError, "Err > Searcher SetQuery: Query -%s- threw an error", str.c_str());
	}

	// Reset the field pattern
	if (p) {
		*(p + 1) = *(cStrFieldPattern + 1);
	}

	SetQuery(query);
}
//------------------------------------------------------------------------------

void Searcher::SetQuery(Lucene::search::Query *inQuery)
{
	_CLDELETE(mQuery);
	mQuery = inQuery;
}
//------------------------------------------------------------------------------

const Lucene::search::Query *Searcher::GetQuery() const
{
	return mQuery;
}
//------------------------------------------------------------------------------

const EntrySet *Searcher::GetEntrySet() const
{
	return mEntrySet;
}
//------------------------------------------------------------------------------

void Searcher::Compute(bool inTraceQuery /*= false*/)
{
	// Reset the result entry set
	delete mEntrySet;

	if (mQuery == NULL) {
		mEntrySet = new EntrySet;
		return;
	}

	/*if (inTraceQuery) {
		UnicodeString field("field");
		const UChar* s = mQuery->toString(field.getTerminatedBuffer());
		UTrace("Searcher::Compute query", s);
		delete [] s;
	}*/

	mQueryResolver = new QueryResolver(this);
	mEntrySet = mQueryResolver->Resolve(mQuery);
	delete mQueryResolver;
	mQueryResolver = NULL;

	if (inTraceQuery) {
		std::string strQuery;
		UnicodeString field("field");
		const UChar* s = mQuery->toString(field.getTerminatedBuffer());
		UChar2String(strQuery, s);
		std::string strEntry(EntrySet2String(*mEntrySet));
		Trace("Searcher::Compute query - %s - %s", strQuery.c_str(), strEntry.c_str());
		delete [] s;
	}
}
//------------------------------------------------------------------------------

/*
Lucene::index::TermEnum* Searcher::GetTermEnum() const
{
}*/
//------------------------------------------------------------------------------

Lucene::index::TermEnum* Searcher::GetTermEnum(const Lucene::index::Term* inTerm) const
{
	const UChar *field = inTerm->field();
	DbNdx *dbNdx = GetFieldDatabase(field);

	// The database may be null if the field is incorrect
	Lucene::index::SegmentTermEnum *segmentTermEnum = new Lucene::index::SegmentTermEnum(dbNdx, field);
	segmentTermEnum->scanTo(inTerm);

	return segmentTermEnum;
}
//------------------------------------------------------------------------------
