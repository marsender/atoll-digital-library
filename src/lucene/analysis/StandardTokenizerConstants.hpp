/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_analysis_standard_StandardTokenizerConstants
#define _lucene_analysis_standard_StandardTokenizerConstants

CL_NS_DEF(analysis)

enum TokenTypes {
	_EOF,
	UNKNOWN,
	ALPHANUM,
	APOSTROPHE,
	ACRONYM,
	COMPANY,
	EMAIL,
	HOST,
	NUM,
	CJK
};

extern const UChar** tokenImage;

CL_NS_END
#endif
