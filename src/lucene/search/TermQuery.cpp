/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "TermQuery.hpp"
//#include "SearchHeader.hpp"
//#include "Scorer.hpp"
//#include "../index/Term.hpp"
//#include "TermScorer.hpp"
//#include "../index/IndexReader.hpp"
#include "../util/StringBuffer.hpp"
#include "../index/Term.hpp"
#include "../index/Terms.hpp"
#include "../../searcher/Searcher.hpp"
#include "../../searcher/QueryResolver.hpp"
#include "unicode/ustring.h"

U_STRING_DECL(cStrClassName, "TermQuery", 9);
CL_NS_DEF(search)
void TermQuery::InitStringDecl() {
	U_STRING_INIT(cStrClassName, "TermQuery", 9);
}
CL_NS_END

CL_NS_USE(index)
CL_NS_DEF(search)
using namespace Atoll;

/** Constructs a query for the term <code>t</code>. */
TermQuery::TermQuery(Term* t)
	:term(_CL_POINTER(t))
{
}

TermQuery::TermQuery(const TermQuery& clone)
	:Query(clone)
{
	this->term = _CL_POINTER(clone.term);
}

TermQuery::~TermQuery()
{
	_CLDECDELETE(term);
}

Query* TermQuery::clone() const
{
	return _CLNEW TermQuery(*this);
}

const UChar *TermQuery::getClassName()
{
	return cStrClassName;
}

const UChar *TermQuery::getQueryName() const
{
	return getClassName();
}

size_t TermQuery::hashCode() const
{
	return 0; // -DC- Similarity::floatToByte(getBoost()) ^ term->hashCode();
}

//added by search highlighter
/*
Term* TermQuery::getTerm(bool pointer) const
{
	if (pointer)
		return _CL_POINTER(term);
	else
		return term;
}
*/

UChar *TermQuery::toString(const UChar *field) const
{
	CL_NS(util)::StringBuffer buffer;
	if (field == NULL || _tcscmp(term->field(), field) != 0) {
		buffer.append(term->field());
		buffer.append(_T(":"));
	}
	buffer.append(term->text());
	if (getBoost() != 1.0f) {
		buffer.append(_T("^"));
		buffer.appendFloat(getBoost(), 1);
	}
	return buffer.toString();
}

bool TermQuery::equals(Query *other) const
{
	if (!(other->instanceOf(TermQuery::getClassName())))
		return false;

	TermQuery* tq = static_cast<TermQuery *>(other);
	return (this->getBoost() == tq->getBoost())
	       && this->term->equals(tq->term);
}

Atoll::EntrySet* TermQuery::Search(Searcher *inSearcher)
{
	EntrySet *entrySet;
	const UChar *str = term->text();
	size_t len = term->textLength();
	const UChar *field = term->field();
	entrySet = inSearcher->mQueryResolver->RevolveTermQuery(field, str, len);

	return entrySet;
}

/*
TermQuery::TermWeight::TermWeight(Searcher* searcher, TermQuery* _this, Term* _term)
{
	this->_this = _this;
	this->_term = _term;
	this->searcher = searcher;
	value = 0;
	idf = 0;
	queryNorm = 0;
	queryWeight = 0;
}
TermQuery::TermWeight::~TermWeight()
{
}

//return a *new* string describing this object
UChar *TermQuery::TermWeight::toString()
{
	int32_t size = _tcslen(_this->getQueryName()) + 10;
	UChar *tmp = _CL_NEWARRAY(UChar, size);//_tcslen(weight())
	_sntprintf(tmp, size, _T("weight(%s)"), _this->getQueryName());
	return tmp;
}

float_t TermQuery::TermWeight::sumOfSquaredWeights()
{
	idf = _this->getSimilarity(searcher)->idf(_term, searcher); // compute idf
	queryWeight = idf * _this->getBoost();             // compute query weight
	return queryWeight * queryWeight;           // square it
}

void TermQuery::TermWeight::normalize(float_t queryNorm)
{
	this->queryNorm = queryNorm;
	queryWeight *= queryNorm;                   // normalize query weight
	value = queryWeight * idf;                  // idf for document
}

Scorer* TermQuery::TermWeight::scorer(IndexReader* reader)
{
	TermDocs* termDocs = reader->termDocs(_term);

	if (termDocs == NULL)
		return NULL;

	return _CLNEW TermScorer(this, termDocs, _this->getSimilarity(searcher),
	                         reader->norms(_term->field()));
}

void TermQuery::TermWeight::explain(IndexReader* reader, int32_t doc, Explanation* result)
{
	UChar buf[LUCENE_SEARCH_EXPLANATION_DESC_LEN];
	UChar *tmp;

	tmp = getQuery()->toString();
	_sntprintf(buf, LUCENE_SEARCH_EXPLANATION_DESC_LEN,
	           _T("weight(%s in %d), product of:"), tmp, doc);
	_CLDELETE_CARRAY(tmp);
	result->setDescription(buf);

	_sntprintf(buf, LUCENE_SEARCH_EXPLANATION_DESC_LEN,
	           _T("idf(docFreq=%d)"), searcher->docFreq(_term));
	Explanation* idfExpl = _CLNEW Explanation(idf, buf);

	// explain query weight
	Explanation* queryExpl = _CLNEW Explanation();
	tmp = getQuery()->toString();
	_sntprintf(buf, LUCENE_SEARCH_EXPLANATION_DESC_LEN,
	           _T("queryWeight(%s), product of:"), tmp);
	_CLDELETE_CARRAY(tmp);
	queryExpl->setDescription(buf);

	Explanation* boostExpl = _CLNEW Explanation(_this->getBoost(), _T("boost"));
	if (_this->getBoost() != 1.0f)
		queryExpl->addDetail(boostExpl);
	else
		_CLDELETE(boostExpl);

	queryExpl->addDetail(idfExpl->clone());

	Explanation* queryNormExpl = _CLNEW Explanation(queryNorm, _T("queryNorm"));
	queryExpl->addDetail(queryNormExpl);

	queryExpl->setValue(_this->getBoost()* // always 1.0
	                    idfExpl->getValue() *
	                    queryNormExpl->getValue());

	// explain field weight
	const UChar *field = _term->field();
	Explanation* fieldExpl = _CLNEW Explanation();

	tmp = _term->toString();
	_sntprintf(buf, LUCENE_SEARCH_EXPLANATION_DESC_LEN,
	           _T("fieldWeight(%s in %d), product of:"), tmp, doc);
	_CLDELETE_CARRAY(tmp);
	fieldExpl->setDescription(buf);

	Scorer* sc = scorer(reader);
	Explanation* tfExpl = _CLNEW Explanation;
	sc->explain(doc, tfExpl);
	_CLDELETE(sc);
	fieldExpl->addDetail(tfExpl);
	fieldExpl->addDetail(idfExpl);

	Explanation* fieldNormExpl = _CLNEW Explanation();
	uint8_t* fieldNorms = reader->norms(field);
	float_t fieldNorm =
	  fieldNorms != NULL ? Similarity::decodeNorm(fieldNorms[doc]) : 0.0f;
	fieldNormExpl->setValue(fieldNorm);

	_sntprintf(buf, LUCENE_SEARCH_EXPLANATION_DESC_LEN,
	           _T("fieldNorm(field=%s, doc=%d)"), field, doc);
	fieldNormExpl->setDescription(buf);
	fieldExpl->addDetail(fieldNormExpl);

	fieldExpl->setValue(tfExpl->getValue() *
	                    idfExpl->getValue() *
	                    fieldNormExpl->getValue());

	//if (queryExpl->getValue() == 1.0f){
	//_CLDELETE(result);
	//    return fieldExpl;
	//}else{
	result->addDetail(queryExpl);
	result->addDetail(fieldExpl);

	// combine them
	result->setValue(queryExpl->getValue() * fieldExpl->getValue());
	//}
}

Weight* TermQuery::_createWeight(Searcher* searcher)
{
	return _CLNEW TermWeight(searcher, this, term);
}
*/

CL_NS_END
