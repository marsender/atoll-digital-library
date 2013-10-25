/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "Term.hpp"
//#include "../util/StringIntern.hpp"
#include "unicode/ustring.h"

U_STRING_DECL(cStrToStringSep, ":", 1);
CL_NS_DEF(index)
void Term::InitStringDecl() {
	U_STRING_INIT(cStrToStringSep, ":", 1);
}
CL_NS_END
//------------------------------------------------------------------------------

CL_NS_USE(util)
CL_NS_DEF(index)

Term::Term()
{
	_field = new UChar[1];
	*_field = 0;
	_text = new UChar[1];
	*_text = 0;
	cachedHashCode = 0;
	mFieldLen = 0;
	mTextLen = 0;
}

Term::Term(const UChar *inStrField, const UChar *inText)
{
	//Func - Constructor.
	//       Constructs a Term with the given field and text. Field and text are not copied
	//       Field and text are deleted in destructor only if intern is false.
	//Pre  - inStrField != NULL and contains the name of the field
	//       inText != NULL and contains the value of the field
	//Post - An instance of Term has been created

	_field = new UChar[1];
	*_field = 0;
	_text = new UChar[1];
	*_text = 0;
	mFieldLen = 0;
	mTextLen = 0;

	set(inStrField, inText);
}

Term::~Term()
{
	//Func - Destructor.
	//Pre  - true
	//Post - The instance has been destroyed. field and text have been deleted

	_CLDELETE_CARRAY(_text);
	_CLDELETE_CARRAY(_field);
}

const UChar *Term::field() const
{
	//Func - Returns the field of this term, an interned string. The field indicates
	//       the part of a document which this term came from.
	//Pre  - true
	//Post - field has been returned

	return _field;
}

const UChar *Term::text() const
{
	//Func - Returns the text of this term.  In the case of words, this is simply the
	//       text of the word.  In the case of dates and other types, this is an
	//       encoding of the object as a string.
	//Pre  - true
	//Post - text has been returned

	return _text;
}


/*
void Term::set(const Term* term, const UChar *inText)
{
	set(term->field(), inText);
}
*/

void Term::set(const UChar *inStrField, const UChar *inText)
{
	//Func - Resets the field and text of a Term.
	//Pre  - inStrField != NULL and contains the name of the field
	//       inText != NULL and contains the value of the field
	//Post - field and text of Term have been reset

	CND_PRECONDITION(inStrField != NULL, "Term field is NULL");
	CND_PRECONDITION(inText != NULL, "Term text is NULL");
	CND_PRECONDITION(inStrField != _field, "Term field is reused");
	CND_PRECONDITION(inText != _text, "Term text is reused");

	size_t len;
	cachedHashCode = 0; // Reset hash

	// Set field
	len = _tcslen(inStrField);
	if (len > mFieldLen) {
		_CLDELETE_ARRAY(_field);
		_field = new UChar[len + 1];
	}
	_tcscpy(_field, inStrField);
	mFieldLen = len;

	// Set text
	len = _tcslen(inText);
	if (len > mTextLen) {
		_CLDELETE_ARRAY(_text);
		_text = new UChar[len + 1];
	}
	_tcscpy(_text, inText);
	mTextLen = len;
}

bool Term::equals(const Term* other) const
{
	if (cachedHashCode != 0 && other->cachedHashCode != 0 && other->cachedHashCode != cachedHashCode)
		return false;

	if (mFieldLen != other->mFieldLen || mTextLen != other->mTextLen)
		return false;

	int32_t ret = _tcscmp(_field, other->_field);
	if (ret == 0)
		ret = _tcscmp(_text, other->_text);

	return (ret == 0);
}

size_t Term::hashCode()
{
	if (cachedHashCode == 0)
		cachedHashCode = Misc::thashCode(_field) + Misc::thashCode(_text, mTextLen);
	return cachedHashCode;
}

int32_t Term::compareTo(const Term* other) const
{
	//Func - Compares two terms, to see if this term belongs before, is equal to or after the argument term.
	//Pre  - other is a reference to another term
	//Post - A negative integer is returned if this term belongs before the argument,
	//       zero is returned if this term is equal to the argument, and a positive integer
	//       if this term belongs after the argument.

	int32_t ret = _tcscmp(_field, other->_field);
	if (ret == 0)
		ret = _tcscmp(_text, other->_text);

	return ret;
}

UChar *Term::toString() const
{
//Func - Forms the contents of Field and term in some kind of tuple notation
//       <field:text>
//Pre  - true
//Post - a string formatted as <field:text> is returned if pre(field) is NULL and
//       text is NULL the returned string will be formatted as <:>

	return CL_NS(util)::Misc::join(_field, cStrToStringSep, _text);
}

CL_NS_END
