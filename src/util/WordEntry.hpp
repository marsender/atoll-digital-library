/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

WordEntry.hpp

*******************************************************************************/

#ifndef __WordEntry_HPP
#define __WordEntry_HPP
//------------------------------------------------------------------------------

#include "Entry.hpp"
#include "unicode/unistr.h"
#include <vector>
#include <string>
using icu::UnicodeString;
//------------------------------------------------------------------------------

namespace Atoll
{

//! Storage class for one word and it's own entry position
class WordEntry
{
public:
	UnicodeString mWrd;
	Entry mEntry;

  WordEntry();
  WordEntry(const UChar *inStr, const Entry &inEntry);
  WordEntry(const UnicodeString &inStr, const Entry &inEntry);

	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! WordEntry vector (theoricaly more efficient than a list)
typedef std::vector <WordEntry> WordEntryVector;
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const WordEntry &inWordEntry);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
