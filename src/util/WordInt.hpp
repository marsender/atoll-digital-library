/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

WordInt.hpp

*******************************************************************************/

#ifndef __WordInt_HPP
#define __WordInt_HPP
//------------------------------------------------------------------------------

#include "unicode/unistr.h"
#include <vector>
#include <string>
//------------------------------------------------------------------------------

namespace Atoll
{

//! Storage class for one word and it's own entry position
class WordInt
{
public:
	UnicodeString mWrd;
	unsigned long mInt;

  WordInt();
  WordInt(const UChar *inStr, unsigned long inInt);
  WordInt(const UnicodeString &inStr, unsigned long inInt);

	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! WordInt vector (theoricaly more efficient than a list)
typedef std::vector <WordInt> WordIntVector;
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const WordInt &inWordInt);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
