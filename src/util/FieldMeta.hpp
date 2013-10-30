/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

FieldMeta.hpp

*******************************************************************************/

#ifndef __FieldMeta_HPP
#define __FieldMeta_HPP
//------------------------------------------------------------------------------

#include "unicode/unistr.h"
#include <vector>
//------------------------------------------------------------------------------

namespace Common
{
	class BinaryBuffer;
}

namespace Atoll
{

enum eTypField {
	eTypFieldNone,
	eTypFieldAll,              //!< All purpose field
	eTypFieldSearch,           //!< Search field
	eTypFieldEnd
};

//! Storage class for a field metadata record
class DEF_Export FieldMeta
{
private:
	UnicodeString mFieldId; //!< Field id

public:
	eTypField mTypField; //!< Field type
	unsigned int mFieldOrder; //!< Field display order
	UnicodeString mFieldTitle; //!< Field title

	FieldMeta(); //!< Constructor
	void Clear(); //!< Clear the content
	void Trim(); //!< Trim the strings
	bool IsValid() const; //!< Check validity
	UnicodeString GetFieldId() const; //!< Get the id
	void SetFieldId(const UnicodeString &inId); //!< Set the id
	void GetFieldKey(UnicodeString &outKey) const; //!< Get the key from type and id
	void GetStrFieldType(std::string &outStrFieldType) const; //!< Get the litteral field type
	void SetFieldType(const std::string &inStrFieldType); //!< Set the field type from a litteral

	void FromBinaryCharBuffer(const char *inBuffer, size_t inSize);
	void FromBinaryBuffer(const Common::BinaryBuffer &inBuffer);
	void ToBinaryBuffer(Common::BinaryBuffer &outBuffer) const;

	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! List of field metadata records
typedef std::vector < FieldMeta > FieldMetaVector;
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const FieldMeta &inFieldMeta);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
