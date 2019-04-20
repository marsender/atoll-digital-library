/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColMeta.hpp

*******************************************************************************/

#ifndef __ColMeta_HPP
#define __ColMeta_HPP
//------------------------------------------------------------------------------

#include "unicode/unistr.h"
#include <string>
#include <vector>
using icu::UnicodeString;
//------------------------------------------------------------------------------

namespace Common
{
	class BinaryBuffer;
}

namespace Atoll
{

//! Storage class for a collection metadata record
class DEF_Export ColMeta
{
public:
	unsigned int mColNum; //!< Collection number
	UnicodeString mColId; //!< Collection id
	UnicodeString mColTitle; //!< Collection title (long name)
	UnicodeString mColDesc; //!< Collection description
	UnicodeString mColSalt; //!< Collection salt (secret string)

	ColMeta(); //!< Constructor
	void Clear(); //!< Clear the content
	void Trim(); //!< Trim the strings

	void FromBinaryCharBuffer(const char *inBuffer, size_t inSize);

	void FromBinaryBuffer(const Common::BinaryBuffer &inBuffer);
	void ToBinaryBuffer(Common::BinaryBuffer &outBuffer) const;

	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! List of collection metadata records
typedef std::vector < ColMeta > ColMetaVector;
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const ColMeta &inColMeta);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
