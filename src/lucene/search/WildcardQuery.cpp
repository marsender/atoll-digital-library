/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "WildcardQuery.hpp"
#include "FilteredTermEnum.hpp"
#include "WildcardTermEnum.hpp"
#include "../index/Term.hpp"
//#include "../util/BitSet.hpp"
//#include "../../searcher/Searcher.hpp"
#include "unicode/ustring.h"

U_STRING_DECL(cStrClassName, "WildcardQuery", 13);
CL_NS_DEF(search)
void WildcardQuery::InitStringDecl() {
	U_STRING_INIT(cStrClassName, "WildcardQuery", 13);
}
CL_NS_END

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)
using namespace Atoll;

WildcardQuery::WildcardQuery(Term* term)
	:MultiTermQuery(term)
{
	//Func - Constructor
	//Pre  - term != NULL
	//Post - Instance has been created

}

WildcardQuery::~WildcardQuery()
{
	//Func - Destructor
	//Pre  - true
	//Post - true

}

const UChar *WildcardQuery::getQueryName() const
{
	//Func - Returns the string "WildcardQuery"
	//Pre  - true
	//Post - The string "WildcardQuery" has been returned
	return getClassName();
}

const UChar *WildcardQuery::getClassName()
{
	return cStrClassName;
}

FilteredTermEnum* WildcardQuery::getEnum(Searcher *inSearcher)
{
	getTerm(false);
	WildcardTermEnum* ret = _CLNEW WildcardTermEnum(inSearcher, getTerm(false));
	return ret;
}

WildcardQuery::WildcardQuery(const WildcardQuery& clone):
		MultiTermQuery(clone)
{
}

Query* WildcardQuery::clone() const
{
	return _CLNEW WildcardQuery(*this);
}
size_t WildcardQuery::hashCode() const
{
	//todo: we should give the query a seeding value... but
	//need to do it for all hascode functions
	return 0; //Similarity::floatToByte(getBoost()) ^ getTerm()->hashCode();
}
bool WildcardQuery::equals(Query *other) const
{
	if (!(other->instanceOf(WildcardQuery::getClassName())))
		return false;

	WildcardQuery* tq = static_cast<WildcardQuery *>(other);
	return (this->getBoost() == tq->getBoost())
	       && getTerm()->equals(tq->getTerm());
}

/*
WildcardFilter::WildcardFilter(Term* term)
{
	this->term = _CL_POINTER(term);
}

WildcardFilter::~WildcardFilter()
{
	_CLDECDELETE(term);
}

WildcardFilter::WildcardFilter(const WildcardFilter& copy) :
		term(_CL_POINTER(copy.term))
{
}

Filter* WildcardFilter::clone() const
{
	return _CLNEW WildcardFilter(*this);
}


UChar *WildcardFilter::toString()
{
	//Instantiate a stringbuffer buffer to store the readable version temporarily
	CL_NS(util)::StringBuffer buffer;
	//check if field equal to the field of prefix
	if (term->field() != NULL) {
		//Append the field of prefix to the buffer
		buffer.append(term->field());
		//Append a colon
		buffer.append(_T(":"));
	}
	//Append the text of the prefix
	buffer.append(term->text());

	//Convert StringBuffer buffer to UChar block and return it
	return buffer.toString();
}

// Returns a BitSet with true for documents which should be permitted in
// search results, and false for those that should not.
BitSet* WildcardFilter::bits(IndexReader* reader)
{
	BitSet* bts = _CLNEW BitSet(reader->maxDoc());

	WildcardTermEnum termEnum (reader, term);
	if (termEnum.term(false) == NULL)
		return bts;

	TermDocs* termDocs = reader->termDocs();
	try {
		do {
			termDocs->seek(&termEnum);

			while (termDocs->next()) {
				bts->set(termDocs->doc());
			}
		}
		while (termEnum.next());
	}
	_CLFINALLY(
	  termDocs->close();
	  _CLDELETE(termDocs);
	  termEnum.close();
	)

	return bts;
}
*/

CL_NS_END
