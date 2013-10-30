/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

InitStringDecl.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "InitStringDecl.hpp"
#include "../lucene/Lucene.hpp"
#include "../lucene/analysis/AnalysisHeader.hpp"
#include "../lucene/analysis/StandardTokenizer.hpp"
#include "../lucene/index/Term.hpp"
#include "../lucene/queryparser/Lexer.hpp"
#include "../lucene/queryparser/QueryParserBase.hpp"
#include "../lucene/search/BooleanQuery.hpp"
#include "../lucene/search/FuzzyQuery.hpp"
#include "../lucene/search/PhraseQuery.hpp"
#include "../lucene/search/PrefixQuery.hpp"
#include "../lucene/search/RangeQuery.hpp"
#include "../lucene/search/TermQuery.hpp"
#include "../lucene/search/WildcardQuery.hpp"
#include "../lucene/util/Misc.hpp"
#include "../lucene/util/StringBuffer.hpp"
//------------------------------------------------------------------------------

void Atoll::InitStringDecl()
{
	Lucene::analysis::Token::InitStringDecl();
	Lucene::analysis::StandardTokenizer::InitStringDecl();
	Lucene::index::Term::InitStringDecl();
	Lucene::queryParser::Lexer::InitStringDecl();
	Lucene::queryParser::QueryParserBase::InitStringDecl();
	Lucene::search::BooleanQuery::InitStringDecl();
	Lucene::search::FuzzyQuery::InitStringDecl();
	Lucene::search::PhraseQuery::InitStringDecl();
	Lucene::search::PrefixQuery::InitStringDecl();
	Lucene::search::RangeQuery::InitStringDecl();
	Lucene::search::TermQuery::InitStringDecl();
	Lucene::search::WildcardQuery::InitStringDecl();
	Lucene::util::Misc::InitStringDecl();
	Lucene::util::StringBuffer::InitStringDecl();
}
//------------------------------------------------------------------------------
