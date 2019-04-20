/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocRecord.hpp

*******************************************************************************/

#ifndef __DocRecord_HPP
#define __DocRecord_HPP
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

//! Storage class for a document record
class DEF_Export DocRecord
{
public:
	UnicodeString mTexte;
	unsigned long mPge, mPos;

	DocRecord();
	void Clear();

	void GetPageKey(UnicodeString &outKey) const;

	void FromBinaryCharBuffer(const char *inBuffer, size_t inSize);
	void FromBinaryBuffer(const Common::BinaryBuffer &inBuffer);
	void ToBinaryBuffer(Common::BinaryBuffer &outBuffer) const;

	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! List of document records
typedef std::vector < DocRecord > DocRecordVector;
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const DocRecord &inDocRecord);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
