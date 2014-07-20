/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocMeta.hpp

*******************************************************************************/

#ifndef __DocMeta_HPP
#define __DocMeta_HPP
//------------------------------------------------------------------------------

#include "unicode/unistr.h"
#include <string>
#include <vector>
//------------------------------------------------------------------------------

namespace Common
{
	class BinaryBuffer;
}

namespace Atoll
{

//! Storage class for a document metadata record
class DEF_Export DocMeta
{
public:
	unsigned int mDocNum;              //!< Document number (used as an id)
	unsigned long mCountPge;           //!< Document page count
	bool mXmlValidation;               //!< Xml validation during parsing ?
	bool mIsIndexed;                   //!< Is the document indexed
	UnicodeString mDocTitle;           //!< Document title
	UnicodeString mUuid;               //!< Document unique id
	UnicodeString mXsltCode;           //!< Document stylesheet code
	std::string mSource;               //!< Document source (file name)
	std::string mFileName;             //!< Document file name
	std::string mDcMetadata;           //!< Document Dublin Core metadata file name
	std::string mIndexerConfig;        //!< Indexer configuration file name
	std::string mRecordBreakerConfig;  //!< Record breaker configuration file name

	DocMeta(unsigned int inDocNum = 0); //!< Constructor
	void Clear(); //!< Clear the content
	void Trim(); //!< Trim the strings

	void FromBinaryCharBuffer(const char *inBuffer, size_t inSize);

	void FromBinaryBuffer(const Common::BinaryBuffer &inBuffer);
	void ToBinaryBuffer(Common::BinaryBuffer &outBuffer) const;

	//! Output metadata in xml format
	void ToXml(UnicodeString &outStr) const;
	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! List of document metadata records
typedef std::vector < DocMeta > DocMetaVector;
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const DocMeta &inDocMeta);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
