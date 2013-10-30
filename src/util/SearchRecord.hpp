/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

SearchRecord.hpp

*******************************************************************************/

#ifndef __SearchRecord_HPP
#define __SearchRecord_HPP
//------------------------------------------------------------------------------

#include "Entry.hpp"
#include "SearchCrit.hpp"
#include <string>
#include <vector>
//------------------------------------------------------------------------------

namespace Common
{
	class BinaryBuffer;
}

namespace Atoll
{

enum eTypSearchRecord {
	eTypSearchRecordNone,
	eTypSearchRecordNormal,             //!< Normal search
	eTypSearchRecordCritOnly,           //!< Search criteria only
	eTypSearchRecordEnd
};

//! Storage class for a search record
class DEF_Export SearchRecord
{
public:
	eTypSearchRecord mTypSearchRecord; //!< Search record type
	unsigned int mValidityIndice; //!< Indice of validity
	std::string mSearchId; //!< Search id = md5 sum of the search criteria vector
	SearchCritVector mSearchCritVector; //!< Search criteria vector
	EntrySet mEntrySet; //!< Search result

	SearchRecord();
	void Clear();
	bool IsValid() const; //!< Check validity
	//void SetSearchId(); //!< Set the search id from the md5 sum of the search criteria vector
	void GetSearchRecordKey(UnicodeString &outKey) const; //!< Get the key from the search record type and id

	//! Unserialise a search record from a buffer
	void FromBinaryCharBuffer(const char *inBuffer, size_t inSize);
	//! Unserialise a search record from a binary buffer
	void FromBinaryBuffer(const Common::BinaryBuffer &inBuffer);
	//! Serialise a search record. The search id will set to the search criteria vector md5 sum
	void ToBinaryBuffer(Common::BinaryBuffer &outBuffer) const;

	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! List of search records
typedef std::vector < SearchRecord > SearchRecordVector;
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const SearchRecord &inSearchRecord);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
