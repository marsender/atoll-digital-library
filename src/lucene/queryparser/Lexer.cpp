/*******************************************************************************

Lexer.cpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#include "../Lucene.hpp"
#include "Lexer.hpp"
#include "TokenList.hpp"
#include "../util/FastCharStream.hpp"
#include "../util/Reader.hpp"
#include "../util/StringBuffer.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

const short cMaxNumLength = 100;

U_STRING_DECL(cStrQueryTokenAnd1, "AND", 3);
U_STRING_DECL(cStrQueryTokenAnd2, "&&", 2);
U_STRING_DECL(cStrQueryTokenOr1, "OR", 2);
U_STRING_DECL(cStrQueryTokenOr2, "||", 2);
U_STRING_DECL(cStrQueryTokenNot, "NOT", 3);
U_STRING_DECL(cStrEscapeChars, "\\+-!():^[]{}\"~*", 15);
CL_NS_DEF(queryParser)
void Lexer::InitStringDecl() {
	U_STRING_INIT(cStrQueryTokenAnd1, "AND", 3);
	U_STRING_INIT(cStrQueryTokenAnd2, "&&", 2);
	U_STRING_INIT(cStrQueryTokenOr1, "OR", 2);
	U_STRING_INIT(cStrQueryTokenOr2, "||", 2);
	U_STRING_INIT(cStrQueryTokenNot, "NOT", 3);
	U_STRING_INIT(cStrEscapeChars, "\\+-!():^[]{}\"~*", 15);
}
CL_NS_END
//------------------------------------------------------------------------------

CL_NS_USE(util)
using namespace Common;

CL_NS_DEF(queryParser)

Lexer::Lexer(QueryParserBase* queryparser, const UChar* query)
{
	//Func - Constructor
	//Pre  - query != NULL and contains the query string
	//Post - An instance of Lexer has been created

	this->queryparser = queryparser;

	CND_PRECONDITION(query != NULL, "query is NULL");

	//The InputStream of Reader must be destroyed in the destructor
	delSR = true;

	StringReader *r = _CLNEW StringReader(query);

	//Check to see if r has been created properly
	CND_CONDITION(r != NULL, "Could not allocate memory for StringReader r");

	//Instantie a FastCharStream instance using r and assign it to reader
	reader = _CLNEW FastCharStream(r);

	//Check to see if reader has been created properly
	CND_CONDITION(reader != NULL, "Could not allocate memory for FastCharStream reader");

	//The InputStream of Reader must be destroyed in the destructor
	delSR = true;
}

Lexer::Lexer(QueryParserBase* queryparser, Reader* source)
{
	//Func - Constructor
	//       Initializes a new instance of the Lexer class with the specified
	//       TextReader to lex.
	//Pre  - Source contains a valid reference to a Reader
	//Post - An instance of Lexer has been created using source as the reader

	this->queryparser = queryparser;

	//Instantie a FastCharStream instance using r and assign it to reader
	reader = _CLNEW FastCharStream(source);

	//Check to see if reader has been created properly
	CND_CONDITION(reader != NULL, "Could not allocate memory for FastCharStream reader");

	//The InputStream of Reader must not be destroyed in the destructor
	delSR  = false;
}


Lexer::~Lexer()
{
	//Func - Destructor
	//Pre  - true
	//Post - if delSR was true the InputStream input of reader has been deleted
	//       The instance of Lexer has been destroyed

	if (delSR) {
		_CLDELETE(reader->input);
	}

	_CLDELETE(reader);
}

void Lexer::Lex(TokenList *outTokenList)
{
	AppAssert(outTokenList != NULL);

	// Add the outQueryToken to the tokens list
	while (true) {
		// Get the next outQueryToken
		QueryToken *queryToken = new QueryToken;
		if (!GetNextToken(queryToken)) {
			delete queryToken;
			break;
		}
		outTokenList->Add(queryToken);
	}

	// The end has been reached so add a final EOF_ outQueryToken
	outTokenList->Add(new QueryToken( QueryToken::EOF_));
}
//------------------------------------------------------------------------------

bool Lexer::GetNextToken(QueryToken* outQueryToken)
{
	while (!reader->Eos()) {
		int ch = reader->GetNext();
		if (ch == -1)
			break;
		// Skipping whitespaces
		if (_istspace(ch) != 0) {
			continue;
		}
		UChar outBuf[2] = {(UChar)ch, 0};
		switch (ch) {
		case '+':
			outQueryToken->set(outBuf, QueryToken::PLUS);
			return true;
		case '-':
			outQueryToken->set(outBuf, QueryToken::MINUS);
			return true;
		case '(':
			outQueryToken->set(outBuf, QueryToken::LPAREN);
			return true;
		case ')':
			outQueryToken->set(outBuf, QueryToken::RPAREN);
			return true;
		case ':':
			outQueryToken->set(outBuf, QueryToken::COLON);
			return true;
		case '!':
			outQueryToken->set(outBuf, QueryToken::NOT);
			return true;
		case '^':
			outQueryToken->set(outBuf, QueryToken::CARAT);
			return true;
		case '~':
			if (_istdigit(reader->Peek())) {
				UChar number[cMaxNumLength];
				ReadIntegerNumber(ch, number, cMaxNumLength);
				outQueryToken->set(number, QueryToken::SLOP);
				return true;
			}
			else {
				outQueryToken->set(outBuf, QueryToken::FUZZY);
				return true;
			}
			break;
		case '"':
			return ReadQuoted(ch, outQueryToken);
		case '[':
			return ReadInclusiveRange(ch, outQueryToken);
		case '{':
			return ReadExclusiveRange(ch, outQueryToken);
		case '<':
			return ReadFullField(ch, outQueryToken);
		case ']':
		case '}':
		case '>':
		case '*':
			gLog.log(eTypLogError, "Err > Lexer: Unrecognized char %d at %d::%d", ch, reader->Column(), reader->Line());
			return false;
		default:
			return ReadTerm(ch, outQueryToken);
		}
	}

	return false;
}
//------------------------------------------------------------------------------

void Lexer::ReadIntegerNumber(const UChar inChar, UChar *outStr, int32_t inLength)
{
	int bp = 0;
	outStr[bp++] = inChar;

	int c = reader->Peek();
	while (c != -1 && _istdigit(c) != 0 && bp < inLength - 1) {
		outStr[bp++] = reader->GetNext();
		c = reader->Peek();
	}
	outStr[bp++] = 0;
}
//------------------------------------------------------------------------------

bool Lexer::ReadInclusiveRange(const UChar inCharPrev, QueryToken* outQueryToken)
{
	int ch = inCharPrev;
	StringBuffer range;
	range.appendChar(ch);

	while (!reader->Eos()) {
		ch = reader->GetNext();
		if (ch == -1)
			break;
		range.appendChar(ch);
		if (ch == ']') {
			outQueryToken->set(range.getBuffer(), 0, range.length(), QueryToken::RANGEIN);
			return true;
		}
	}
	gLog.log(eTypLogError, "Err > Lexer: Unterminated inclusive range at %d::%d", reader->Column(), reader->Line());

	return false;
}
//------------------------------------------------------------------------------

bool Lexer::ReadExclusiveRange(const UChar inCharPrev, QueryToken* outQueryToken)
{
	int ch = inCharPrev;
	StringBuffer range;
	range.appendChar(ch);

	while (!reader->Eos()) {
		ch = reader->GetNext();
		if (ch == -1)
			break;
		range.appendChar(ch);
		if (ch == '}') {
			outQueryToken->set(range.getBuffer(), 0, range.length(), QueryToken::RANGEEX);
			return true;
		}
	}
	gLog.log(eTypLogError, "Err > Lexer: Unterminated exclusive range at %d::%d", reader->Column(), reader->Line());

	return false;
}
//------------------------------------------------------------------------------

bool Lexer::ReadFullField(const UChar inCharPrev, QueryToken* outQueryToken)
{
	int ch = inCharPrev;
	StringBuffer range;
	range.appendChar(ch);

	while (!reader->Eos()) {
		ch = reader->GetNext();
		if (ch == -1)
			break;
		range.appendChar(ch);
		if (ch == '>') {
			outQueryToken->set(range.getBuffer(), 0, range.length(), QueryToken::FULLFIELD);
			return true;
		}
	}
	gLog.log(eTypLogError, "Err > Lexer: Unterminated exclusive range at %d::%d", reader->Column(), reader->Line());

	return false;
}
//------------------------------------------------------------------------------

bool Lexer::ReadQuoted(const UChar inCharPrev, QueryToken* outQueryToken)
{
	int ch = inCharPrev;
	StringBuffer quoted;
	quoted.appendChar(ch);

	while (!reader->Eos()) {
		ch = reader->GetNext();
		if (ch == -1)
			break;
		quoted.appendChar(ch);
		if (ch == '"') {
			outQueryToken->set(quoted.getBuffer(), 0, quoted.length(), QueryToken::QUOTED);
			return true;
		}
	}
	gLog.log(eTypLogError, "Err > Lexer: Unterminated string at %d::%d", reader->Column(), reader->Line());

	return false;
}
//------------------------------------------------------------------------------

bool Lexer::ReadTerm(const UChar inCharPrev, QueryToken* outQueryToken)
{
	int ch = inCharPrev;
	bool completed = false;
	int32_t asteriskCount = 0;
	bool hasQuestion = false;

	StringBuffer val;
	UChar buf[3]; //used for readescaped

	while (true) {
		switch (ch) {
		case -1:
			break;
		case '\\': {
			if (ReadEscape(ch, buf))
				val.append(buf);
			else
				return false;
		}
		break;

		case LUCENE_WILDCARDTERMENUM_WILDCARD_STRING:
			asteriskCount++;
			val.appendChar(ch);
			break;
		case LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR:
			hasQuestion = true;
			val.appendChar(ch);
			break;
		case '\n':
		case '\t':
		case ' ':
		case '+':
		case '-':
		case '!':
		case '(':
		case ')':
		case ':':
		case '^':
		case '[':
		case ']':
		case '{':
		case '}':
		case '<':
		case '>':
		case '~':
		case '"':
			// create new QueryToken
			reader->UnGet();
			completed = true;
			break;
		default:
			val.appendChar(ch);
			break;
			// end of switch
		}

		if (completed || ch == -1 || reader->Eos())
			break;
		else
			ch = reader->GetNext();
	}

	// Create new QueryToken
	const UChar *uStr = val.getBuffer();
	if (hasQuestion)
		outQueryToken->set(uStr, QueryToken::WILDTERM);
	else if (asteriskCount == 1 && uStr[val.length() - 1] == '*')
		outQueryToken->set(uStr, QueryToken::PREFIXTERM);
	else if (asteriskCount > 0)
		outQueryToken->set(uStr, QueryToken::WILDTERM);
	else if (_tcsicmp(uStr, cStrQueryTokenAnd1) == 0 || _tcscmp(uStr, cStrQueryTokenAnd2) == 0)
		outQueryToken->set(uStr, QueryToken::AND_);
	else if (_tcsicmp(uStr, cStrQueryTokenOr1) == 0 || _tcscmp(uStr, cStrQueryTokenOr2) == 0)
		outQueryToken->set(uStr, QueryToken::OR);
	else if (_tcsicmp(uStr, cStrQueryTokenNot) == 0)
		outQueryToken->set(uStr, QueryToken::NOT);
	else {
		bool isnum = true;
		int32_t nlen = val.length();
		for (int32_t i = 0;i < nlen; ++i) {
			UChar ch = uStr[i];
			if (_istalpha(ch)) {
				isnum = false;
				break;
			}
		}
		if (isnum)
			outQueryToken->set(val.getBuffer(), 0, val.length(), QueryToken::NUMBER);
		else
			outQueryToken->set(val.getBuffer(), 0, val.length(), QueryToken::TERM);
	}

	return true;
}
//------------------------------------------------------------------------------

bool Lexer::ReadEscape(UChar inCharPrev, UChar *outBuf)
{
	UChar ch = inCharPrev;
	int bp = 0;
	outBuf[bp++] = ch;

	ch = reader->GetNext();
	int32_t idx = u_strcspn(outBuf, cStrEscapeChars);
	if (idx == 0) {
		outBuf[bp++] = ch;
		outBuf[bp++] = 0;
		return true;
	}
	gLog.log(eTypLogError, "Err > Lexer: Unrecognized escape sequence at %d::%d", reader->Column(), reader->Line());

	return false;
}
//------------------------------------------------------------------------------

CL_NS_END
