/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_index_SegmentTermEnum_
#define _lucene_index_SegmentTermEnum_

#include "Terms.hpp" // For TermEnum
//#include "FieldInfos.hpp"
//#include "TermInfo.hpp"
#include "unicode/unistr.h"

namespace Atoll {
	class DbNdx;
}

CL_NS_DEF(index)

//class TermInfo;

/**
 * SegmentTermEnum is an enumeration of all Terms and TermInfos
 */
class SegmentTermEnum: public TermEnum
{
private:
	UChar *mField;
	Atoll::DbNdx *mDbNdx;

	Term* _term; //!< points to the current Term in the enumeration
	//Term* prev; //!< The previous current

	int64_t size; //!< The size of the enumeration
	int64_t position; //!< The position of the current (term) in the enumeration

	//! Read the term at the current position
	void readTerm(Term *ioTerm);

protected:

	// Constructor. The instance is created by cloning all properties of clone
	//SegmentTermEnum(const SegmentTermEnum& clone);

public:
	//! Constructor
	SegmentTermEnum(Atoll::DbNdx *inDbNdx, const UChar *inStrField);

	//! Destructor
	~SegmentTermEnum();

	//! Moves the current of the set to the next in the set
	bool next();

	//! Returns a pointer to the current term.
	Term* term();

	//! Returns the current term.
	Term* term(bool pointer);

	//! Scan for a term and move the iterator to it's position
	void scanTo(const Term *term);

	//! Closes the enumeration to further activity, freeing resources.
	void close();

	//! Returns a clone of this instance
	//SegmentTermEnum* clone() const;

	int64_t GetSize() { return size; }

	const char* getObjectName() {
		return SegmentTermEnum::getClassName();
	}

	static const char* getClassName() {
		return "SegmentTermEnum";
	}
};
CL_NS_END
#endif
