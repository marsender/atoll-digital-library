/*******************************************************************************

Analyzers.cpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#include "../Lucene.hpp"
#include "Analyzers.hpp"
#include "../util/StringBuffer.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

CL_NS_USE(util)

UChar* lucene_tcslwr(UChar* str)
{
	UChar* ret = str;
	for (; *str; str++) *str = _totlower(*str);
	return ret;
}

CL_NS_DEF(analysis)

CharTokenizer::CharTokenizer(Reader* in) :
		Tokenizer(in),
		offset(0),
		bufferIndex(0),
		dataLen(0),
		ioBuffer(NULL)
{
	buffer[0] = 0;
}

UChar CharTokenizer::normalize(const UChar c) const
{
	return c;
}

bool CharTokenizer::next(Token* token)
{
	int32_t length = 0;
	int32_t start = offset;
	while (true) {
		UChar c;
		offset++;
		if (bufferIndex >= dataLen) {
			dataLen = input->read(ioBuffer, LUCENE_IO_BUFFER_SIZE);
			if (dataLen == -1)
				dataLen = 0;
			bufferIndex = 0;
		}
		if (dataLen <= 0) {
			if (length > 0)
				break;
			else
				return false;
		}
		else
			c = ioBuffer[bufferIndex++];
		if (isTokenChar(c)) {                       // if it's a token UChar

			if (length == 0)			  // start of token
				start = offset - 1;

			buffer[length++] = normalize(c);          // buffer it, normalized

			if (length == LUCENE_MAX_WORD_LEN)		  // buffer overflow!
				break;

		}
		else if (length > 0)			  // at non-Letter w/ chars
			break;					  // return 'em

	}
	buffer[length] = 0;
	token->set( buffer, start, start + length);
	return true;
}

bool LetterTokenizer::isTokenChar(const UChar c) const
{
	return _istalpha(c) != 0;
}


UChar LowerCaseTokenizer::normalize(const UChar chr) const
{
	// Warning: LowerCaseTokenizer performs the function of LetterTokenizer and LowerCaseFilter together
	return _totlower(chr);
}

bool WhitespaceTokenizer::isTokenChar(const UChar c) const
{
	return _istspace(c) == 0; //(return true if NOT a space)
}

TokenStream* LetterAnalyzer::tokenStream(const UChar *fieldName, Reader* reader)
{
	return _CLNEW LetterTokenizer(reader);
}

TokenStream* WhitespaceAnalyzer::tokenStream(const UChar *fieldName, Reader* reader)
{
	return _CLNEW WhitespaceTokenizer(reader);
}

TokenStream* SimpleAnalyzer::tokenStream(const UChar *fieldName, Reader* reader)
{
	return _CLNEW LowerCaseTokenizer(reader);
}

bool LowerCaseFilter::next(Token* t)
{
	if (!input->next(t))
		return false;

	// Lower case the token term
	//stringCaseFold(t->_termText);
	UnicodeString uStr(t->_termText);
	uStr.toLower();
	t->setText(uStr.getTerminatedBuffer());

	return true;
}

/*
StopFilter::StopFilter(TokenStream* in, bool deleteTokenStream, const UChar** stopWords):
		TokenFilter(in, deleteTokenStream),
		table(_CLNEW CLSetList<const UChar*>(false))
{
	fillStopTable(table, stopWords);
}

void StopFilter::fillStopTable(CLSetList<const UChar*>* stopTable,
                               const UChar** stopWords)
{
	for (int32_t i = 0; stopWords[i] != NULL; i++)
		stopTable->insert(stopWords[i]);
}

bool StopFilter::next(Token* token)
{
	// return the first non-stop word found
	while (input->next(token)) {
		if (table->find(token->_termText) == table->end()) {
			return true;
		}
	}

	// reached EOS -- return nothing
	return false;
}
*/

/*
StopAnalyzer::StopAnalyzer(): stopTable(false)
{
	StopFilter::fillStopTable(&stopTable, ENGLISH_STOP_WORDS);
}
StopAnalyzer::~StopAnalyzer()
{
}
StopAnalyzer::StopAnalyzer( const UChar** stopWords)
{
	StopFilter::fillStopTable(&stopTable, stopWords);
}
TokenStream* StopAnalyzer::tokenStream(const UChar *fieldName, Reader* reader)
{
	return _CLNEW StopFilter(_CLNEW LowerCaseTokenizer(reader), true, &stopTable);
}

const UChar *StopAnalyzer::ENGLISH_STOP_WORDS[]  = {
	_T("a"), _T("an"), _T("and"), _T("are"), _T("as"), _T("at"), _T("be"), _T("but"), _T("by"),
	_T("for"), _T("if"), _T("in"), _T("into"), _T("is"), _T("it"),
	_T("no"), _T("not"), _T("of"), _T("on"), _T("or"), _T("s"), _T("such"),
	_T("t"), _T("that"), _T("the"), _T("their"), _T("then"), _T("there"), _T("these"),
	_T("they"), _T("this"), _T("to"), _T("was"), _T("will"), _T("with"), NULL
};
*/

/*
PerFieldAnalyzerWrapper::PerFieldAnalyzerWrapper(Analyzer* defaultAnalyzer):
		analyzerMap(true, true)
{
	this->defaultAnalyzer = defaultAnalyzer;
}
PerFieldAnalyzerWrapper::~PerFieldAnalyzerWrapper()
{
	analyzerMap.clear();
	_CLDELETE(defaultAnalyzer);
}

void PerFieldAnalyzerWrapper::addAnalyzer(const UChar *fieldName, Analyzer* analyzer)
{
	analyzerMap.put(STRDUP_TtoT(fieldName), analyzer);
}

TokenStream* PerFieldAnalyzerWrapper::tokenStream(const UChar *fieldName, Reader* reader)
{
	Analyzer* analyzer = (fieldName == NULL ? defaultAnalyzer : analyzerMap.get(fieldName));
	if (analyzer == NULL) {
		analyzer = defaultAnalyzer;
	}

	return analyzer->tokenStream(fieldName, reader);
}
*/

bool ISOLatin1AccentFilter::next(Token* token)
{
	if (input->next(token)) {
		int32_t l = token->termTextLength();
		const UChar *chars = token->termText();
		bool doProcess = false;
		for (int32_t i = 0; i < l; ++i) {
//#ifdef _UCS2
			if (chars[i] >= 0xC0 && chars[i] <= 0x178) {
//#else
//			if ((chars[i] >= 0xC0 && chars[i] <= 0xFF) || chars[i] < 0) {
//#endif
				doProcess = true;
				break;
			}

		}
		if (!doProcess) {
			return true;
		}

		StringBuffer output(l*2);
		for (int32_t j = 0; j < l; ++j) {
//#ifdef _UCS2
			UChar c = chars[j];
//#else
//			unsigned char c = chars[j];
//#endif
			switch (c) {
			case 0xC0 : // �
			case 0xC1 : // �
			case 0xC2 : // �
			case 0xC3 : // �
			case 0xC4 : // �
			case 0xC5 : // �
				output.appendChar('A');
				break;
			case 0xC6 : // �
				output.append(_T("AE"));
				break;
			case 0xC7 : // �
				output.appendChar('C');
				break;
			case 0xC8 : // �
			case 0xC9 : // �
			case 0xCA : // �
			case 0xCB : // �
				output.appendChar('E');
				break;
			case 0xCC : // �
			case 0xCD : // �
			case 0xCE : // �
			case 0xCF : // �
				output.appendChar('I');
				break;
			case 0xD0 : // �
				output.appendChar('D');
				break;
			case 0xD1 : // �
				output.appendChar('N');
				break;
			case 0xD2 : // �
			case 0xD3 : // �
			case 0xD4 : // �
			case 0xD5 : // �
			case 0xD6 : // �
			case 0xD8 : // �
				output.appendChar('O');
				break;
			case 0xDE : // �
				output.append(_T("TH"));
				break;
			case 0xD9 : // �
			case 0xDA : // �
			case 0xDB : // �
			case 0xDC : // �
				output.appendChar('U');
				break;
			case 0xDD : // �
				output.appendChar('Y');
				break;
			case 0xE0 : // �
			case 0xE1 : // �
			case 0xE2 : // �
			case 0xE3 : // �
			case 0xE4 : // �
			case 0xE5 : // �
				output.appendChar('a');
				break;
			case 0xE6 : // �
				output.append(_T("ae"));
				break;
			case 0xE7 : // �
				output.appendChar('c');
				break;
			case 0xE8 : // �
			case 0xE9 : // �
			case 0xEA : // �
			case 0xEB : // �
				output.appendChar('e');
				break;
			case 0xEC : // �
			case 0xED : // �
			case 0xEE : // �
			case 0xEF : // �
				output.appendChar('i');
				break;
			case 0xF0 : // �
				output.appendChar('d');
				break;
			case 0xF1 : // �
				output.appendChar('n');
				break;
			case 0xF2 : // �
			case 0xF3 : // �
			case 0xF4 : // �
			case 0xF5 : // �
			case 0xF6 : // �
			case 0xF8 : // �
				output.appendChar('o');
				break;
			case 0xDF : // �
				output.append(_T("ss"));
				break;
			case 0xFE : // �
				output.append(_T("th"));
				break;
			case 0xF9 : // �
			case 0xFA : // �
			case 0xFB : // �
			case 0xFC : // �
				output.appendChar('u');
				break;
			case 0xFD : // �
			case 0xFF : // �
				output.appendChar('y');
				break;
//#ifdef _UCS2
			case 0x152 : // �
				output.append(_T("OE"));
				break;
			case 0x153 : // �
				output.append(_T("oe"));
				break;
			case 0x178 : // �
				output.appendChar('Y');
				break;
//#endif
			default :
				output.appendChar(c);
				break;
			}
		}
		token->setText(output.getBuffer());
		return true;
	}
	return false;
}

TokenStream* KeywordAnalyzer::tokenStream(const UChar *fieldName, CL_NS(util)::Reader* reader) {
	return _CLNEW KeywordTokenizer(reader);
}

KeywordTokenizer::KeywordTokenizer(CL_NS(util)::Reader* input, int bufferSize):
		Tokenizer(input) {
	this->done = false;
	if (bufferSize < 0)
		this->bufferSize = DEFAULT_BUFFER_SIZE;
}

KeywordTokenizer::~KeywordTokenizer() {
}

bool KeywordTokenizer::next(Token* token) {
	if (!done) {
		done = true;
		int32_t rd;
		const UChar *buffer = 0;
		while (true) {
			rd = input->read(buffer, bufferSize);
			if (rd == -1)
				break;
			token->growBuffer(token->_termTextLen + rd + 1);

			int32_t cp = rd;
			if (token->_termTextLen + cp > (int32_t)token->bufferLength())
				cp = token->bufferLength() -  token->_termTextLen;
			_tcsncpy(token->_termText + token->_termTextLen, buffer, cp);
			token->_termTextLen += rd;
		}
		token->_termText[token->_termTextLen] = 0;
		token->set(token->_termText, 0, token->_termTextLen);
		return true;
	}
	return false;
}

LengthFilter::LengthFilter(TokenStream* in, int _min, int _max):
		TokenFilter(in) {
	this->_min = _min;
	this->_max = _max;
}

bool LengthFilter::next(Token* token) {
	// return the first non-stop word found
	while (input->next(token)) {
		int len = token->termTextLength();
		if (len >= _min && len <= _max)
			return true;
		// note: else we ignore it but should we index each part of it?
	}
	// reached EOS -- return null
	return false;
}

CL_NS_END
