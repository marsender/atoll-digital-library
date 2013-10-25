/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "Terms.hpp"
#include "Term.hpp"

CL_NS_USE(util)

CL_NS_DEF(index)

Term* TermEnum::term(bool pointer)
{
	Term* ret = term();
	if (!pointer)
		ret->__cl_decref();
	return ret;
}

bool TermEnum::skipTo(Term* target)
{
	do {
		if (!next())
			return false;
	}
	while (target->compareTo(term(false)) > 0);
	return true;
}

CL_NS_END
