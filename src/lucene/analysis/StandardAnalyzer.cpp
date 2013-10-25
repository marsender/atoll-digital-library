/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "StandardAnalyzer.hpp"
#include "../util/VoidMap.hpp"
#include "../util/Reader.hpp"
#include "AnalysisHeader.hpp"
#include "Analyzers.hpp"
#include "StandardFilter.hpp"
#include "StandardTokenizer.hpp"

CL_NS_USE(util)
CL_NS_USE(analysis)

CL_NS_DEF(analysis)

StandardAnalyzer::StandardAnalyzer()
	//:stopSet(false)
{
	//StopFilter::fillStopTable( &stopSet, CL_NS(analysis)::StopAnalyzer::ENGLISH_STOP_WORDS);
}

StandardAnalyzer::StandardAnalyzer(const UChar** stopWords)
	//:stopSet(false)
{
	//StopFilter::fillStopTable(&stopSet, stopWords);
}

StandardAnalyzer::~StandardAnalyzer()
{
}

TokenStream* StandardAnalyzer::tokenStream(const UChar *fieldName, Reader* reader)
{
	TokenStream* ret = _CLNEW StandardTokenizer(reader);

	// Add the filters
	ret = _CLNEW StandardFilter(ret, true);
	//ret = _CLNEW LowerCaseFilter(ret, true);
	//ret = _CLNEW StopFilter(ret, true, &stopSet);

	return ret;
}

CL_NS_END
