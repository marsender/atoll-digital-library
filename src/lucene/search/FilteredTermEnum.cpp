/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "FilteredTermEnum.hpp"
#include "../index/Term.hpp"
#include "../index/Terms.hpp"

CL_NS_USE(index)

CL_NS_DEF(search)

FilteredTermEnum::FilteredTermEnum()
{
	//Func - Constructor
	//Pre  - true
	//Post - Instance has been created

	mCurrentTerm = NULL;
	mActualEnum = NULL;
}

FilteredTermEnum::~FilteredTermEnum()
{
	//Func - Destructor
	//Pre  - true
	//Post - The instance has been destroyed

	close();
}

/*
int32_t FilteredTermEnum::docFreq() const
{
	//Func - Returns the docFreq of the current Term in the enumeration.
	//Pre  - next() must have been called at least once
	//Post - if actualEnum is NULL result is -1 otherwise the frequencey is returned

	if (mActualEnum == NULL) {
		return -1;
	}
	return mActualEnum->docFreq();
}
*/

bool FilteredTermEnum::next()
{
	//Func - Increments the enumeration to the next element.
	//Pre  - true
	//Post - Returns True if the enumeration has been moved to the next element otherwise false

	// The actual enumerator is not initialized!
	if (mActualEnum == NULL)
		return false;

	// Finalize the current term and reset it to NULL
	_CLDECDELETE(mCurrentTerm);

	// Iterate through the enumeration
	while (mCurrentTerm == NULL) {
		if (endEnum())
			return false;
		if (mActualEnum->next()) {
			// Order term not to return reference ownership here
			Term* term = mActualEnum->term(false);
			// Compare the retrieved term
			if (termCompare(term)) {
				// Matched so finalize the current
				_CLDECDELETE(mCurrentTerm);
				// Get a reference to the matched term
				mCurrentTerm = _CL_POINTER(term);
				return true;
			}
		}
		else
			return false;
	}
	_CLDECDELETE(mCurrentTerm);
	mCurrentTerm = NULL;

	return false;
}

Term* FilteredTermEnum::term()
{
	//Func - Returns the current Term in the enumeration.
	//Pre  - next() must have been called at least once
	//       pointer is true or false
	//Post - if pre(pointer) is true the reference counter of current term is increased
	//       and current Term is returned otherwise current term is only returned

	return _CL_POINTER(mCurrentTerm);
}
Term* FilteredTermEnum::term(bool pointer)
{
	if (pointer)
		return _CL_POINTER(mCurrentTerm);
	else
		return mCurrentTerm;
}

void FilteredTermEnum::close()
{
	//Func - Closes the enumeration to further activity, freeing resources.
	//Pre  - true
	//Post - The Enumeration has been closed

	// Check if actualEnum is valid
	if (mActualEnum) {
		// Close the enumeration
		mActualEnum->close();
	}

	// Destroy the enumeration
	_CLDELETE(mActualEnum);

	// Destroy current term
	_CLDECDELETE(mCurrentTerm);
}

void FilteredTermEnum::setEnum(TermEnum* inActualEnum)
{
	//Func - Sets the actual Enumeration
	//Pre  - actualEnum != NULL
	//Post - The instance has been created

	CND_PRECONDITION(inActualEnum != NULL, "ActualEnum is NULL");

	_CLDELETE(mActualEnum);

	mActualEnum = inActualEnum;

	// Find the first term that matches
	// Ordered term not to return reference ownership here.
	Term* term = mActualEnum->term(false);
	if (term != NULL && termCompare(term)) {
		_CLDECDELETE(mCurrentTerm);
		mCurrentTerm = _CL_POINTER(term);
	}
	else {
		next();
	}
}

CL_NS_END
