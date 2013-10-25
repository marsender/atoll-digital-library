/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_index_Term_
#define _lucene_index_Term_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

//#include "terms.hpp"
#include "../util/RefCount.hpp"
#include "../util/Misc.hpp"
//#include "../util/StringIntern.hpp"

CL_NS_DEF(index)

/**
A Term represents a word from text.  This is the unit of search.  It is
composed of two elements, the text of the word, as a string, and the name of
the field that the text occured in, an interned string.

Note that terms may represent more than words from text fields, but also
things like dates, email addresses, urls, etc.

IMPORTANT NOTE:
Term inherits from the template class LUCENE_REFBASE which tries to do
some garbage collection by counting the references an instance has. As a result
of this construction you MUST use _CLDECDELETE(obj) when you want to delete an
of Term!

ABOUT intrn

intrn indicates if field and text are interned or not. Interning of Strings is the process of
converting duplicated strings to shared ones.

*/
class Term: public CL_NS(util)::RefCount
{
private:
	UChar *_field;
	UChar *_text;
	size_t cachedHashCode;
	size_t mFieldLen; //!< Field length
	size_t mTextLen; //!< Text length

public:
	///Constructs a blank term
	Term();

	//! Constructor. Constructs a Term with the given field and text
	Term(const UChar *fld, const UChar *txt);

	// Uses the specified fieldTerm's field. this saves on intern'ing time.
	//Term(const Term* fieldTerm, const UChar *txt);

	///Destructor.
	~Term();

	//! Init string declarations
	static void InitStringDecl();

	///Returns the field of this term, an interned string. The field indicates
	///the part of a document which this term came from.
	const UChar *field() const; ///<returns reference

	///Returns the text of this term.  In the case of words, this is simply the
	///text of the word.  In the case of dates and other types, this is an
	///encoding of the object as a string.
	const UChar *text() const; ///<returns reference

	//void set(const Term* term, const UChar *txt);

	//! Sets the field and text
	void set(const UChar *inStrField, const UChar *inText);

	/** Compares two terms, returning a negative integer if this
	term belongs before the argument, zero if this term is equal to the
	argument, and a positive integer if this term belongs after the argument.	
	The ordering of terms is first by field, then by text.*/
	int32_t compareTo(const Term* other) const;

	//! Compares two terms, returning true if they have the same field and text
	bool equals(const Term* other) const;

	size_t textLength() const {
		return mTextLen;
	}

	//! Forms the contents of Field and term in some kind of tuple notation <field:text>
	UChar *toString() const;

	size_t hashCode();

	class Equals: public CL_NS_STD(binary_function)<const Term*, const Term*, bool>
	{
		public:
			bool operator()(const Term* val1, const Term* val2) const {
				return val1->equals(val2);
			}
	};

	/*
	class Compare: LUCENE_BASE, public CL_NS(util)::Compare::_base //<Term*>
	{
	public:
		bool operator()(Term* t1, Term* t2) const {
			return (t1->compareTo(t2) < 0);
		}
		size_t operator()(Term* t) const {
			return t->hashCode();
		}
	};
	*/
};

CL_NS_END
#endif
