/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "SegmentTermEnum.hpp"
#include "Terms.hpp"
#include "Term.hpp"
#include "../../common/BufUtil.hpp"
#include "../../database/DbNdx.hpp"
#include "unicode/ustring.h"
#include <stdlib.h>
#include <string.h>

CL_NS_DEF(index)

SegmentTermEnum::SegmentTermEnum(Atoll::DbNdx *inDbNdx, const UChar *inStrField)
	:mDbNdx(inDbNdx)
{
	position = -1;
	size = mDbNdx ? mDbNdx->Count() : 0;
	_term = _CLNEW Term;
	//prev = NULL;
	mField = stringDuplicate(inStrField);
}

/*
SegmentTermEnum::SegmentTermEnum(const SegmentTermEnum& clone)
{
	//Func - Constructor
	//       The instance is created by cloning all properties of clone
	//Pre  - clone holds a valid reference to SegmentTermEnum
	//Post - An instance of SegmentTermEnum with the same properties as clone

	position = clone.position;
	size = clone.size;

	_term = NULL;
	if (clone._term)
		_term = _CLNEW Term(clone._term->field(), clone._term->text());

	//prev = NULL;
	//if (clone.prev)
	//	prev = _CLNEW Term(clone.prev->field(), clone.prev->text());
}
*/

SegmentTermEnum::~SegmentTermEnum()
{
	//Func - Destructor
	//Pre  - true
	//Post - The instance has been destroyed. If this instance was a clone
	//       then the inputstream is closed and deleted too.

	_CLDECDELETE(_term);
	//_CLDECDELETE(prev);
	_CLDELETE_CARRAY(mField);
}

Term* SegmentTermEnum::term()
{
	//Func - Returns the current term.
	//Pre  - pointer is true or false and indicates if the reference counter
	//       of term must be increased or not
	//       next() must have been called once!
	//Post - pointer = true -> term has been returned with an increased reference counter
	//       pointer = false -> term has been returned

	return _CL_POINTER(_term);
}

Term* SegmentTermEnum::term(bool pointer)
{
	if (pointer)
		return _CL_POINTER(_term);
	else
		return _term;
}

void SegmentTermEnum::close()
{
	//Func - Closes the enumeration to further activity, freeing resources.
	//Pre  - true
	//Post - The inputStream input has been closed

	//input->close();
}

/*
SegmentTermEnum* SegmentTermEnum::clone() const
{
	//Func - Returns a clone of this instance
	//Pre  - true
	//Post - An clone of this instance has been returned

	return _CLNEW SegmentTermEnum(*this);
}
*/

bool SegmentTermEnum::next()
{
	//Func - Moves the current of the set to the next in the set
	//Post - If the end has been reached false is returned otherwise the term has
	//       become the next Term in the enumeration

	// Increase position by and and check if the end has been reached
	if (position++ >= size - 1) {
		_CLDECDELETE(_term);
		return false;
	}

	// Delete the previous enumerated term
	/*
	Term* tmp = NULL;
	if (prev != NULL) {
		int32_t usage = prev->__cl_refcount;
		if (usage > 1) {
			_CLDECDELETE(prev); //todo: tune other places try and delete its term
		}
		else
			tmp = prev; //we are going to re-use this term
	}
	// Prev becomes the current enumerated term
	prev = _term;
	*/

	// Create a new term as it is ref counted
	_CLDECDELETE(_term);
	_term = _CLNEW Term;
	readTerm(_term);

	return true;
}

void SegmentTermEnum::scanTo(const Term *ioTerm)
{
	//Func - Scan for Term without allocating new Terms
	//Pre  - ioTerm != NULL
	//Post - The iterator ioTerm has been moved to the position where Term is expected to be
	//       in the enumeration

	if (mDbNdx == NULL) {
		position = size;
		_CLDECDELETE(_term);
		return;
	}

	bool wantGetWordRecNum = true;
	if (wantGetWordRecNum) {
		UnicodeString word(ioTerm->text());
		unsigned long recNum = mDbNdx->GetWordRecNum(word);
		if (recNum == DEF_LongMax) {
			position = size;
			_CLDECDELETE(_term);
			return;
		}
		position = recNum;
		readTerm(_term);
	}

	while (ioTerm->compareTo(_term) > 0 && next()) {
	}
}

void SegmentTermEnum::readTerm(Term *ioTerm)
{
	AppAssert(mDbNdx);

	UnicodeString word;
	unsigned long recNum = (unsigned long)position;
	mDbNdx->GetWord(word, recNum);
	ioTerm->set(mField, word.getTerminatedBuffer());
}

CL_NS_END
