/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

WordEntries.hpp

*******************************************************************************/

#ifndef __WordEntries_HPP
#define __WordEntries_HPP
//------------------------------------------------------------------------------

#include "Entry.hpp"
#include "unicode/unistr.h"
#include <set>
#include <string>
//------------------------------------------------------------------------------

namespace Atoll
{

//! Storage class for one word and it's entry set
class WordEntries
{
public:
	UnicodeString mWrd;
	EntrySet mEntrySet;

  WordEntries(const UChar *inStr = NULL);
  void AddEntry(const Entry &inEntry);
  void AddEntry_ConstHack(const Entry &inEntry) const;
  void AddEntries_ConstHack(const EntrySet &inEntrySet) const;
  //const EntrySet *GetEntrySet() const { return &mEntrySet; }

	std::string ToString() const;
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const WordEntries &inWordEntries);

//! WordEntries comparison function by word
struct fCmpWordEntries : std::binary_function<WordEntries, WordEntries, bool>
{
  bool operator() (const WordEntries& w1, const WordEntries& w2) const;
};
//------------------------------------------------------------------------------

//! WordEntries Set with comparison function by word
typedef std::set <WordEntries, fCmpWordEntries, std::allocator<WordEntries> > WordEntriesSet;
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
