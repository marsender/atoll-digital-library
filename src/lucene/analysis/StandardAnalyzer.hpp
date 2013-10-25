/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_analysis_standard_StandardAnalyzer
#define _lucene_analysis_standard_StandardAnalyzer

#include "Analyzers.hpp"
//#include "../util/VoidMap.hpp"

CL_NS_DEF(util)
class Reader;
CL_NS_END

CL_NS_DEF(analysis)

/** Represents a standard analyzer. */
class StandardAnalyzer : public Analyzer
{
private:
	//CL_NS(util)::CLSetList<const UChar*> stopSet;

public:
	/** Builds an analyzer.*/
	StandardAnalyzer();

	/** Builds an analyzer with the given stop words. */
	StandardAnalyzer( const UChar** stopWords);

	~StandardAnalyzer();


	/**
	* Constructs a StandardTokenizer filtered by a
	* StandardFilter, a LowerCaseFilter and a StopFilter.
	*/
	TokenStream* tokenStream(const UChar *fieldName, CL_NS(util)::Reader* reader)
	;
};
CL_NS_END
#endif
