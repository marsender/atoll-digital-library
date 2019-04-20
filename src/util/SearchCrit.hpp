/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

SearchCrit.hpp

*******************************************************************************/

#ifndef __SearchCrit_HPP
#define __SearchCrit_HPP
//------------------------------------------------------------------------------

#include "unicode/unistr.h"
#include <vector>
using icu::UnicodeString;
//------------------------------------------------------------------------------

// Common::BinaryBuffer declaration
namespace Common
{
	class BinaryBuffer;
}
//------------------------------------------------------------------------------

namespace Atoll
{

//! Storage class for one search criteria
class DEF_Export SearchCrit
{
private:
	UnicodeString mIdCrit; //!< Criteria Id = Index name

public:
	UnicodeString mWrd; //!< Criteria value

  SearchCrit();
	//! Copy constructor
  SearchCrit(const SearchCrit &inSearchCrit);

	UnicodeString GetIdCrit() const; //!< Get the id
	void SetIdCrit(const char *inId); //!< Set the id
	void SetIdCrit(const UnicodeString &inId); //!< Set the id

	void Clear();

	std::string ToString() const;
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const SearchCrit &inSearchCrit);

// SearchCrit Vector
typedef std::vector < SearchCrit > SearchCritVector;
//------------------------------------------------------------------------------

//! Convert a search criteria to a string
void SearchCritToString(std::string &outStr, const SearchCrit &inSearchCrit);
//! Convert a string to a search criteria
void StringToSearchCrit(const std::string &inStr, SearchCrit &outSearchCrit);
//! Convert a search crit vector to a string
void SearchCritVectorToString(std::string &outStr, const SearchCritVector &inSearchCritVector);
//! Serialize a search crit vector to a binary buffer
size_t SearchCritVectorToBuf(Common::BinaryBuffer &outBuffer, const SearchCritVector &inSearchCritVector);
//! Unserialize a binary buffer to a search crit vector
size_t BufToSearchCritVector(SearchCritVector &outSearchCritVector, const void *inData);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
