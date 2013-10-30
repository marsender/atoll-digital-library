/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Token.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Token.hpp"
//#include "../common/UnicodeUtil.hpp"
//#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;

Token::Token(const std::string &inElem)
{
	/*
	unsigned int len = strlen(inStr);
  UChar *s = new UChar[len + 1];
	u_uastrcpy(s, inStr);
	mTypToken = StaticGetTypToken(s);
	delete [] s;
	*/

	mTypToken = StaticGetTypToken(inElem);
}
//------------------------------------------------------------------------------

/*
Token::Token(const UChar *inStr)
{
	mTypToken = StaticGetTypToken(inStr);
}*/
//------------------------------------------------------------------------------

eTypToken Token::StaticGetTypToken(const std::string &inStr)
{
	if (inStr == "not") {
		return eTypTokenNote;
	}
	else if (inStr == "nli") {
		return eTypTokenNli;
	}

	return eTypTokenDefault;

  /*
	UChar *s = new UChar[10];
	u_uastrcpy(s, "not");

	if (u_strcmp(inStr, s) == 0) {
		delete [] s;
		return eTypTokenNote;
	}

	delete [] s;
	return eTypTokenDefault;
	*/
}
//------------------------------------------------------------------------------
